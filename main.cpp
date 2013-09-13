#include <iostream>
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE(emptyTest)
{
}

int main(int argc, char **argv)
{
	if (boost::unit_test::unit_test_main([](){ return true; }, argc, argv) != 0) {
		return -1;
	}

	cout << "Hello World!" << endl;
	return 0;
}

