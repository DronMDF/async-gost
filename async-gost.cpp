
#include "async-gost.hpp"

using namespace std;

future<ContextReply> async_cfb_encrypt(const vector<uint8_t> &data, const vector<uint8_t> &/*key*/,
	const vector<uint8_t> &/*iv*/)
{
	return async([&]{
		ContextReply reply;
		reply.data.assign(data.begin(), data.end());
		return reply;
	});
}
