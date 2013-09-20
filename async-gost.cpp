
#include "async-gost.hpp"
#include <cstring>
#include "GostGenericEngine.h"

using namespace std;

 vector<uint8_t> gost_encrypt_cfb(const vector<uint8_t> &data, const vector<uint8_t> &key, const vector<uint8_t> &iv)
{
	GostGenericEngine engine;
	memcpy(&engine.key[0], &key[0], 32);
	memcpy(&engine.iv[0], &iv[0], 8);

	const size_t size = data.size();
	vector<uint8_t> out(size);

	uint32_t i[2];
	for (size_t index = 0; index < size; index += 8) {
		engine.encrypt();
		memcpy(&i[0], &data[index], min(size - index, 8U));
		engine.iv[0] ^= i[0];
		engine.iv[1] ^= i[1];
		memcpy(&out[index], &i[0], min(size - index, 8U));
	}

	return out;
}

future<ContextReply> async_cfb_encrypt(const vector<uint8_t> &data, const vector<uint8_t> &key,
	const vector<uint8_t> &iv)
{
	return async([data, key, iv]{
		ContextReply reply;
		reply.data = gost_encrypt_cfb(data, key, iv);
		return reply;
	});
}
