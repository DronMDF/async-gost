
#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <queue>
#include <tbb/concurrent_queue.h>
#include "async-gost.h"
#include "upp11.h"

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

void fixed_loader(const seconds &interval)
{
	vector<uint8_t> source(10000 * 8);
	vector<uint8_t> key(32);

	default_random_engine generator(system_clock::now().time_since_epoch().count());
	generate(source.begin(), source.end(), generator);
	generate(key.begin(), key.end(), generator);

	const high_resolution_clock::time_point finish = high_resolution_clock::now() + interval;
	while (high_resolution_clock::now() < finish) {
		UserContext context;
		// Сюда только круглые данные
		context.context = async_ecb_encrypt(source, key);
		context.final = false;
		futures.push(context);
	}

	// Посылаем сигнальный блок
	UserContext context;
	context.final = true;
	context.context = async_ecb_encrypt(source, key);
	futures.push(context);
}

vector<size_t> infinity_retriver(int count)
{
	while (futures.empty()) {
		sleep(1);
	}

	vector<size_t> bench;
	int ongone = count;
	while (ongone > 0) {
		size_t processed = 0;
		const high_resolution_clock::time_point iterstop = high_resolution_clock::now() + seconds(1);
		while (high_resolution_clock::now() < iterstop && ongone > 0) {
			UserContext context;
			futures.pop(context);
			ongone -= context.final ? 1 : 0;
			const auto value = context.context.get();
			processed += value.data.size();
		}

		bench.push_back(processed * 8);
	}

	return bench;
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

	return encrypted * 8 / interval.count();
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

	return encrypted * 8 / interval.count();
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

	return encrypted * 8 / interval.count();
}

int main(int, char **)
{
	if (upp11::TestCollection::runAllTests(time(0), false, true) != 0) {
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
//	auto rate1 = rr1.get() + rr2.get() + rr3.get();
//	cout << "loaded: " << rate1 / 1000 << " Kbit/sec" << endl;

	futures.set_capacity(10000);
	thread infinity(infinity_loader, interval);
	//thread fixed(fixed_loader, interval);
	auto rr = async(infinity_retriver, 1);
	const auto rate = rr.get();
	//fixed.join();
	infinity.join();

	for (const auto r: rate) {
		cout << "rate: " << r / 1000 << "Kbit/sec" << endl;
	}

	return 0;
}
