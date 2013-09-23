
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
		if (eq.size() < 1000) {
			eq.push(async_cfb_encrypt(data, key, iv));
		} else {
			const auto value = eq.back().get();
			encrypted += value.size();
			eq.pop();
		}
	}
	return encrypted;
}

int main(int argc, char **argv)
{
	if (boost::unit_test::unit_test_main([](){ return true; }, argc, argv) != 0) {
		return -1;
	}

	add_crypto_thread();

	const auto interval = seconds(10);

	auto rr = async(encrypt_loader, ref(interval));
	auto loaded = rr.get();

	cout << "loaded: " << loaded / interval.count() * 8 / 1000 << " Kbit/sec" << endl;

	return 0;
}
