
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

vector<uint8_t> gost_imit(const vector<uint8_t> &data, const vector<uint8_t> &key)
{
	CryptoEngineGeneric engine;
	memcpy(&engine.key[0], &key[0], 32);
	memset(&engine.iv[0], 0, 8);

	const size_t size = data.size();
	for (size_t index = 0; index < size; index += 8) {
		uint32_t i[2] = {0, 0};
		memcpy(&i[0], &data[index], min(size - index, size_t(8)));
		engine.iv[0] ^= i[0];
		engine.iv[1] ^= i[1];
		engine.imit();
	}

	vector<uint8_t> out(8);
	memcpy(&out[0], &engine.iv[0], 8);
	return out;
}

BOOST_AUTO_TEST_CASE(testGostShouldImit)
{
	// Given
	const vector<uint8_t> text = {
		0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
		0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
		0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
		0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
		0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
		0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28,
		0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
		0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38,
		0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40,
		0x4F, 0x4E, 0x4D, 0x4C, 0x4B, 0x4A, 0x49, 0x48,
		0x57, 0x56, 0x55, 0x54, 0x53, 0x52, 0x51, 0x50,
		0x5F, 0x5E, 0x5D, 0x5C, 0x5B, 0x5A, 0x59, 0x58,
		0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x60,
		0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x69, 0x68,
		0x77, 0x76, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70,
		0x7F, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x78,
		0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80,
		0x8F, 0x8E, 0x8D, 0x8C, 0x8B, 0x8A, 0x89, 0x88,
		0x97, 0x96, 0x95, 0x94, 0x93, 0x92, 0x91, 0x90,
		0x9F, 0x9E, 0x9D, 0x9C, 0x9B, 0x9A, 0x99, 0x98,
		0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1, 0xa0,
		0xaF, 0xaE, 0xaD, 0xaC, 0xaB, 0xaA, 0xa9, 0xa8,
		0xb7, 0xb6, 0xb5, 0xb4, 0xb3, 0xb2, 0xb1, 0xb0,
		0xbF, 0xbE, 0xbD, 0xbC, 0xbB, 0xbA, 0xb9, 0xb8,
		0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0xc2, 0xc1, 0xc0,
		0xcF, 0xcE, 0xcD, 0xcC, 0xcB, 0xcA, 0xc9, 0xc8,
		0xd7, 0xd6, 0xd5, 0xd4, 0xd3, 0xd2, 0xd1, 0xd0,
		0xdF, 0xdE, 0xdD, 0xdC, 0xdB, 0xdA, 0xd9, 0xd8,
		0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1, 0xe0,
		0xeF, 0xeE, 0xeD, 0xeC, 0xeB, 0xeA, 0xe9, 0xe8,
		0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
		0xfF, 0xfE, 0xfD, 0xfC, 0xfB, 0xfA, 0xf9, 0xf8 };
	const vector<uint8_t> key = {
		0x04, 0x75, 0xF6, 0xE0, 0x50, 0x38, 0xFB, 0xFA,
		0xD2, 0xC7, 0xC3, 0x90, 0xED, 0xB3, 0xCA, 0x3D,
		0x15, 0x47, 0x12, 0x42, 0x91, 0xAE, 0x1E, 0x8A,
		0x2F, 0x79, 0xCD, 0x9E, 0xD2, 0xBC, 0xEF, 0xBD };
	// When
	const auto imit = gost_imit(text, key);
	// Them
	const vector<uint8_t> expected = { 0x54, 0x8F, 0x73, 0x46 };
	BOOST_REQUIRE_EQUAL_COLLECTIONS(imit.begin(), imit.begin() + expected.size(),
					expected.begin(), expected.end());
}

// Это основной интерфейс библиотеки
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

