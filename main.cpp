
#include <chrono>
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

mutex flock;
condition_variable fcond;
queue<future<vector<uint8_t>>> futures;

void data_loader()
{
	while(true) {
		unique_lock<mutex> guard(flock);
		fcond.wait(guard, []{ return futures.size() < 1000; });
		futures.push(async(launch::deferred, []{ return vector<uint8_t>(1000000, 255); }));
		fcond.notify_one();
	}
}

int main(int argc, char **argv)
{
	if (boost::unit_test::unit_test_main([](){ return true; }, argc, argv) != 0) {
		return -1;
	}

	thread(data_loader).detach();

	const auto interval = chrono::seconds(10); //minutes(1);
	const chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
	const chrono::high_resolution_clock::time_point finish = start + interval;
	unsigned loaded = 0;
	while (chrono::high_resolution_clock::now() < finish) {
		unique_lock<mutex> guard(flock);
		fcond.wait(guard, []{ return !futures.empty(); });
		const auto value = futures.front().get();
		futures.pop();
		loaded += value.size();
		fcond.notify_one();
	}

	cout << "loaded: " << loaded / chrono::duration_cast<chrono::seconds>(interval).count() * 8
	     << ' ' << futures.size()
	     << ' ' << chrono::duration_cast<chrono::microseconds>(finish - start).count()<< endl;

	return 0;
}

