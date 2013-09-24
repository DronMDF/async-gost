
#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <queue>
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>
#include "async-gost.hpp"

using namespace std;
using namespace std::chrono;

void encrypt_test()
{
	// Given
	const vector<uint8_t> text = {
		0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0x33, 0x33, 0x33,
		0x33, 0x33, 0x33, 0x33, 0xCC, 0xCC, 0xCC, 0xCC 	};
	const vector<uint8_t> key = {
		0x04, 0x75, 0xF6, 0xE0, 0x50, 0x38, 0xFB, 0xFA,
		0xD2, 0xC7, 0xC3, 0x90, 0xED, 0xB3, 0xCA, 0x3D,
		0x15, 0x47, 0x12, 0x42, 0x91, 0xAE, 0x1E, 0x8A,
		0x2F, 0x79, 0xCD, 0x9E, 0xD2, 0xBC, 0xEF, 0xBD };
	const vector<uint8_t> iv = {
		0x2A, 0x80, 0xA7, 0xC3, 0xFF, 0xA8, 0xE3, 0x47, };
	// When
	auto result_context = async_cfb_encrypt(text, key, iv);
	auto result = result_context.get().data;

	// Then
	const vector<uint8_t> expected = {
		0x3B, 0x4C, 0x22, 0x43, 0x10, 0xBD, 0x64, 0x4B,
		0x99, 0xEB, 0x48, 0x55, 0xD9, 0x6C, 0xAA, 0x0F };
	if (!equal(result.begin(), result.end(), expected.begin())) {
		throw runtime_error("Шифратор сломался");
	}

	cout << "Шифратор работает хорошо." << endl;
}

size_t encrypt_loader(const seconds &interval)
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
		encrypted += value.size();
		eq.pop();
	}

	return encrypted;
}

int main(int argc, char **argv)
{
	if (boost::unit_test::unit_test_main([](){ return true; }, argc, argv) != 0) {
		return -1;
	}

	add_crypto_thread();

	encrypt_test();

	const auto interval = seconds(30);

	auto rr1 = async(launch::async, encrypt_loader, ref(interval));
	auto rr2 = async(launch::async, encrypt_loader, ref(interval));
	auto loaded = rr1.get() + rr2.get();

	cout << "loaded: " << loaded / interval.count() * 8 / 1000 << " Kbit/sec" << endl;

	return 0;
}
