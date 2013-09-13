#include <future>
#include <iostream>
#include <thread>
#include <queue>
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE(emptyTest)
{
}

queue<future<vector<uint8_t>>> futures;

void data_loader()
{
	while(true) {
		if(futures.size() < 1000) {
			futures.push(async([](){ return vector<uint8_t>(1000, 255); }));
		}
		this_thread::yield();
	}
}

int main(int argc, char **argv)
{
	if (boost::unit_test::unit_test_main([](){ return true; }, argc, argv) != 0) {
		return -1;
	}

	thread(data_loader).detach();

	unsigned loaded = 0;
	while(true) {
		if (!futures.empty()) {
			const auto value = futures.front().get();
			futures.pop();
			loaded += value.size();
			if (loaded % 1000000 == 0) {
				cout << "loaded: " << loaded << ' ' << futures.size() << endl;
			}
		}
	}

	return 0;
}

