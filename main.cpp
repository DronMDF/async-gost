
#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <queue>
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>
#include "async-gost.h"

using namespace std;
using namespace std::chrono;

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

	// Прогоним тесты без потоков
	crypto_self_test();

	add_crypto_thread();

	// И тесты с потоками
	crypto_self_test();

	const auto interval = seconds(30);

	auto rr1 = async(launch::async, cfb_encrypt_loader, ref(interval));
	auto rr2 = async(launch::async, cfb_decrypt_loader, ref(interval));
	auto rr3 = async(launch::async, ecb_encrypt_loader, ref(interval));
	auto loaded = rr1.get() + rr2.get() + rr3.get();

	cout << "loaded: " << loaded / interval.count() * 8 / 1000 << " Kbit/sec" << endl;

	return 0;
}
