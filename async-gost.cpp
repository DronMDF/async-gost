
#include "async-gost.h"
#include <cstring>
#include <boost/test/unit_test.hpp>
#include <tbb/concurrent_queue.h>
#include "CryptoEngineGeneric.h"
#include "CryptoRequest.h"
#include "CryptoRequestCFBDecrypt.h"
#include "CryptoRequestCFBEncrypt.h"
#include "CryptoRequestECBEncrypt.h"
#include "CryptoRequestImit.h"

using namespace std;

static vector<thread> crypto_encrypt_threads;
static tbb::concurrent_bounded_queue<shared_ptr<CryptoRequest>> crypto_encrypt_tasks;

void crypto_thread_encrypt()
{
	CryptoEngineGeneric engine;
	shared_ptr<CryptoRequest> request;
	while(true) {
		if (!request) {
			crypto_encrypt_tasks.pop(request);
			request->init(&engine.slot);
		}

		request->load(&engine.slot);
		engine.encrypt();
		request->save(&engine.slot);

		if (request->isDone()) {
			request->submit();
			request.reset();
		}
	}
}

static vector<thread> crypto_imit_threads;
static tbb::concurrent_bounded_queue<shared_ptr<CryptoRequest>> crypto_imit_tasks;

void crypto_thread_imit()
{
	CryptoEngineGeneric engine;
	shared_ptr<CryptoRequest> request;
	while(true) {
		if (!request) {
			crypto_imit_tasks.pop(request);
			request->init(&engine.slot);
		}

		request->load(&engine.slot);
		engine.imit();
		request->save(&engine.slot);

		if (request->isDone()) {
			request->submit();
			request.reset();
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

future<ContextReply> async_encrypt(const shared_ptr<CryptoRequest> &request)
{
	if (crypto_encrypt_threads.empty()) {
		// Режим без выделенных потоков
		return async([request]{
			CryptoEngineGeneric engine;
			request->init(&engine.slot);
			while(!request->isDone()) {
				request->load(&engine.slot);
				engine.encrypt();
				request->save(&engine.slot);
			}
			request->submit();
			return request->get_future().get();
		});
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
		return async([request]{
			CryptoEngineGeneric engine;
			request->init(&engine.slot);
			while(!request->isDone()) {
				request->load(&engine.slot);
				engine.imit();
				request->save(&engine.slot);
			}
			request->submit();
			return request->get_future().get();
		});
	}

	crypto_imit_tasks.push(request);
	return request->get_future();
}

