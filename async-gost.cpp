
#include "async-gost.h"
#include <cstring>
#include <tbb/concurrent_queue.h>
#include "CpuSupport.h"
#include "CryptoEngineGeneric.h"
#include "CryptoEngineSSSE3.h"
#include "CryptoRequest.h"
#include "CryptoRequestCFBDecrypt.h"
#include "CryptoRequestCFBEncrypt.h"
#include "CryptoRequestECBEncrypt.h"
#include "CryptoRequestImit.h"
#include "CryptoRequestNull.h"

using namespace std;

static vector<thread> crypto_encrypt_threads;
static tbb::concurrent_bounded_queue<shared_ptr<CryptoRequest>> crypto_encrypt_tasks;

void crypto_thread_encrypt()
{
	shared_ptr<CryptoEngine> engine = make_shared<CryptoEngineGeneric>();

	if (cpu_support_ssse3()) {
		// WTF: В зависимости от опций компиляции этот шифратор может использовать avx
		engine = make_shared<CryptoEngineSSSE3>();
	}

	vector<shared_ptr<CryptoRequest>> request(engine->getSlotCount());
	while(true) {
		int active_slots = engine->getSlotCount();
		for (unsigned s = 0; s < engine->getSlotCount(); s++) {
			if (!request[s]) {
				if (crypto_encrypt_tasks.try_pop(request[s])) {;
					request[s]->init(engine->getSlot(s));
				} else {
					request[s] = make_shared<CryptoRequestNull>();
					active_slots--;
				}
			}

			request[s]->load(engine->getSlot(s));
		}

		if (active_slots == 0) {
			// Заданий нет - ждем до первого задания
			crypto_encrypt_tasks.pop(request[0]);
			request[0]->init(engine->getSlot(0));
			request[0]->load(engine->getSlot(0));
		}

		engine->encrypt();

		for (unsigned s = 0; s < engine->getSlotCount(); s++) {
			request[s]->save(engine->getSlot(s));

			if (request[s]->isDone()) {
				request[s]->submit();
				request[s].reset();
			}
		}
	}
}

static vector<thread> crypto_imit_threads;
static tbb::concurrent_bounded_queue<shared_ptr<CryptoRequest>> crypto_imit_tasks;

void crypto_thread_imit()
{
	CryptoEngineGeneric engine;
	vector<shared_ptr<CryptoRequest>> request(engine.getSlotCount());
	while(true) {
		for (unsigned s = 0; s < engine.getSlotCount(); s++) {
			if (!request[s]) {
				crypto_imit_tasks.pop(request[s]);
				request[s]->init(engine.getSlot(s));
			}

			request[s]->load(engine.getSlot(s));
		}

		engine.imit();

		for (unsigned s = 0; s < engine.getSlotCount(); s++) {
			request[s]->save(engine.getSlot(s));

			if (request[s]->isDone()) {
				request[s]->submit();
				request[s].reset();
			}
		}
	}
}

void add_crypto_thread(crypto_engine_t type)
{
	switch (type) {
		case CRYPTO_ENGINE_ENCRYPT_GENERIC:
			crypto_encrypt_threads.push_back(thread(crypto_thread_encrypt));
			crypto_encrypt_threads.back().detach();
			break;
		case CRYPTO_ENGINE_IMIT_GENERIC:
			crypto_imit_threads.push_back(thread(crypto_thread_imit));
			crypto_imit_threads.back().detach();
			break;
		default:
			throw logic_error("Некорректный тип движка");
	}
}

static
ContextReply async_generic_action(const shared_ptr<CryptoRequest> &request, void (CryptoEngine::* action)())
{
	CryptoEngineGeneric engine;
	request->init(engine.getSlot(0));
	while(!request->isDone()) {
		request->load(engine.getSlot(0));
		(engine.*action)();
		request->save(engine.getSlot(0));
	}
	request->submit();
	return request->get_future().get();
}

static
future<ContextReply> async_encrypt(const shared_ptr<CryptoRequest> &request)
{
	if (request->isDone()) {
		// Пустой запрос игнорируем.
		request->submit();
		return request->get_future();
	}

	if (crypto_encrypt_threads.empty()) {
		return async(async_generic_action, request, &CryptoEngine::encrypt);
	}

	crypto_encrypt_tasks.push(request);
	return request->get_future();
}

future<ContextReply> async_cfb_encrypt(const vector<uint8_t> &data, const vector<uint8_t> &key,
	const vector<uint8_t> &iv)
{
	return async_encrypt(make_shared<CryptoRequestCFBEncrypt>(data, key, iv));
}

future<ContextReply> async_cfb_decrypt(const vector<uint8_t> &data, const vector<uint8_t> &key,
	const vector<uint8_t> &iv)
{
	return async_encrypt(make_shared<CryptoRequestCFBDecrypt>(data, key, iv));
}

future<ContextReply> async_ecb_encrypt(const vector<uint8_t> &data, const vector<uint8_t> &key)
{
	return async_encrypt(make_shared<CryptoRequestECBEncrypt>(data, key));
}

future<ContextReply> async_imit(const vector<uint8_t> &data, const vector<uint8_t> &key)
{
	auto request = make_shared<CryptoRequestImit>(data, key);

	if (crypto_imit_threads.empty()) {
		return async(async_generic_action, request, &CryptoEngine::imit);
	}

	crypto_imit_tasks.push(request);
	return request->get_future();
}
