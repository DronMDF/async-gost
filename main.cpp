
#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <queue>
#include <boost/thread.hpp>
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>
#include <tbb/concurrent_queue.h>
#include "async-gost.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE(emptyTest)
{
}

tbb::concurrent_bounded_queue<shared_future<ContextReply>> futures;

void data_loader()
{
	futures.set_capacity(1000);
	vector<uint8_t> data(1500, 255);
	vector<uint8_t> key(32, 128);
	vector<uint8_t> iv(8, 0);

	while(!boost::this_thread::interruption_requested()) {
		futures.push(async_cfb_encrypt(data, key, iv));
	}
}

int main(int argc, char **argv)
{
	if (boost::unit_test::unit_test_main([](){ return true; }, argc, argv) != 0) {
		return -1;
	}

	boost::thread loader(data_loader);

	const auto interval = chrono::seconds(10); //minutes(1);
	const chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
	const chrono::high_resolution_clock::time_point finish = start + interval;
	unsigned loaded = 0;
	while (chrono::high_resolution_clock::now() < finish) {
		shared_future<ContextReply> result;
		futures.pop(result);
		const auto value = result.get();
		loaded += value.size();
	}

	cout << "loaded: " << loaded / chrono::duration_cast<chrono::seconds>(interval).count() * 8
	     << ' ' << futures.size()
	     << ' ' << chrono::duration_cast<chrono::microseconds>(finish - start).count()<< endl;

	loader.interrupt();

	// Чтобы тред спокойно умер - его надо поднять из ожидания (костыльно немного)
	shared_future<ContextReply> result;
	futures.pop(result);

	loader.join();
	return 0;
}

