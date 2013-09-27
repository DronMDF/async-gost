
#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <queue>
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>
#include <tbb/concurrent_queue.h>
#include "async-gost.h"

using namespace std;
using namespace std::chrono;

struct UserContext {
	bool final;
	shared_future<ContextReply> context;
};

static tbb::concurrent_bounded_queue<UserContext> futures;

void infinity_loader(const seconds &interval)
{
	vector<uint8_t> source(2000);
	default_random_engine generator(system_clock::now().time_since_epoch().count());
	generate(source.begin(), source.end(), generator);
	size_t next_block_size = size_t(source[0]) * 6; // 0 - 1536

	const high_resolution_clock::time_point finish = high_resolution_clock::now() + interval;
	while (high_resolution_clock::now() < finish) {
		const vector<uint8_t> data(source.begin(), source.begin() + next_block_size);
		const vector<uint8_t> key(source.begin() + next_block_size, source.begin() + next_block_size + 32);
		const vector<uint8_t> iv(source.begin() + next_block_size + 32, source.begin() + next_block_size + 40);

		uint8_t mode = size_t(source[next_block_size + 40]) % 3;
		UserContext context;
		switch (mode) {
			case 0:
				if (data.size() % 8 == 0) {
					// Сюда только круглые данные
					context.context = async_ecb_encrypt(data, key);
					break;
				}
			case 1:
				context.context = async_cfb_encrypt(data, key, iv);
				break;
			case 2:
				context.context = async_cfb_decrypt(data, key, iv);
				break;
		}

		context.final = false;
		futures.push(context);
		next_block_size = size_t(source[next_block_size + 40]) * 6; // 0 - 1536
	}

	// Посылаем сигнальный блок
	UserContext context;
	context.final = true;
	context.context = async_ecb_encrypt(source, vector<uint8_t>(source.begin(), source.begin() + 32));
	futures.push(context);
}

size_t infinity_retriver(int count)
{
	while (futures.empty()) {
		sleep(1);
	}
	const high_resolution_clock::time_point start = high_resolution_clock::now();
	int ongone = count;
	size_t processed = 0;
	while (ongone > 0) {
		UserContext context;
		futures.pop(context);
		ongone -= context.final ? 1 : 0;
		const auto value = context.context.get();
		processed += value.data.size();
	}

	const auto period = high_resolution_clock::now() - start;
	return processed * 8 / duration_cast<seconds>(period).count();
}

size_t cfb_encrypt_loader(const seconds &interval)
{
	size_t encrypted = 0;
	queue<future<ContextReply>> eq;
	// Данные для шифрования
	const vector<uint8_t> data(1500, 255);
	const vector<uint8_t> key(32, 128);
	const vector<uint8_t> iv(8, 0);

	const high_resolution_clock::time_point finish = high_resolution_clock::now() + interval;
	while (high_resolution_clock::now() < finish) {
		while (eq.size() < 100) {
			eq.push(async_cfb_encrypt(data, key, iv));
		}

		const auto value = eq.back().get();
		encrypted += value.data.size();
		eq.pop();
	}

	return encrypted;
}

size_t cfb_decrypt_loader(const seconds &interval)
{
	size_t encrypted = 0;
	queue<future<ContextReply>> eq;
	// Данные для шифрования
	const vector<uint8_t> data(1500, 212);
	const vector<uint8_t> key(32, 128);
	const vector<uint8_t> iv(8, 0);

	const high_resolution_clock::time_point finish = high_resolution_clock::now() + interval;
	while (high_resolution_clock::now() < finish) {
		while (eq.size() < 100) {
			eq.push(async_cfb_decrypt(data, key, iv));
		}

		const auto value = eq.back().get();
		encrypted += value.data.size();
		eq.pop();
	}

	return encrypted;
}

size_t ecb_encrypt_loader(const seconds &interval)
{
	size_t encrypted = 0;
	queue<future<ContextReply>> eq;
	// Данные для шифрования
	const vector<uint8_t> data(1496, 123);
	const vector<uint8_t> key(32, 128);

	const high_resolution_clock::time_point finish = high_resolution_clock::now() + interval;
	while (high_resolution_clock::now() < finish) {
		while (eq.size() < 100) {
			eq.push(async_ecb_encrypt(data, key));
		}

		const auto value = eq.back().get();
		encrypted += value.data.size();
		eq.pop();
	}

	return encrypted;
}

int main(int argc, char **argv)
{
	if (boost::unit_test::unit_test_main([](){ return true; }, argc, argv) != 0) {
		return -1;
	}

	crypto_self_test();
	cout << "Тестирование без потоков прошло успешно;" << endl;

	add_crypto_thread(CRYPTO_ENGINE_ENCRYPT_GENERIC);
	//add_crypto_thread(CRYPTO_ENGINE_ENCRYPT_GENERIC);
	//add_crypto_thread(CRYPTO_ENGINE_ENCRYPT_GENERIC);
	//add_crypto_thread(CRYPTO_ENGINE_IMIT_GENERIC);

	// И тесты с потоками
	crypto_self_test();
	cout << "Тестирование с потоками прошло успешно;" << endl;

	const auto interval = seconds(30);

//	auto rr1 = async(launch::async, cfb_encrypt_loader, ref(interval));
//	auto rr2 = async(launch::async, cfb_decrypt_loader, ref(interval));
//	auto rr3 = async(launch::async, ecb_encrypt_loader, ref(interval));
//	auto loaded = rr1.get() + rr2.get() + rr3.get();
//	cout << "loaded: " << loaded / interval.count() * 8 / 1000 << " Kbit/sec" << endl;

	futures.set_capacity(1000);
	thread loader1(infinity_loader, interval);
	//thread loader2(infinity_loader, interval);
	auto rr = async(infinity_retriver, 1);
	auto rate = rr.get();
	loader1.join();
	//loader2.join();
	cout << "loaded: " << rate / 1000 << " Kbit/sec" << endl;

	return 0;
}
