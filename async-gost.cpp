
#include "async-gost.hpp"
#include <cstring>
#include <boost/test/unit_test.hpp>
#include "GostGenericEngine.h"

using namespace std;

vector<uint8_t> gost_encrypt_cfb(const vector<uint8_t> &data, const vector<uint8_t> &key,
				  const vector<uint8_t> &iv)
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
		memcpy(&out[index], &engine.iv[0], min(size - index, 8U));
	}

	return out;
}

vector<uint8_t> gost_imit(const vector<uint8_t> &data, const vector<uint8_t> &key)
{
	GostGenericEngine engine;
	memcpy(&engine.key[0], &key[0], 32);
	memset(&engine.iv[0], 0, 8);

	const size_t size = data.size();
	for (size_t index = 0; index < size; index += 8) {
		uint32_t i[2] = {0, 0};
		memcpy(&i[0], &data[index], min(size - index, 8U));
		engine.iv[0] ^= i[0];
		engine.iv[1] ^= i[1];
		engine.imit();
	}

	vector<uint8_t> out(8);
	memcpy(&out[0], &engine.iv[0], 8);
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

BOOST_AUTO_TEST_CASE(testGostShouldEncrypt)
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
	const auto cypher = gost_encrypt_cfb(text, key, iv);
	// Then
	const vector<uint8_t> expected = {
		0x3B, 0x4C, 0x22, 0x43, 0x10, 0xBD, 0x64, 0x4B,
		0x99, 0xEB, 0x48, 0x55, 0xD9, 0x6C, 0xAA, 0x0F };
	BOOST_REQUIRE_EQUAL_COLLECTIONS(cypher.begin(), cypher.end(),
					expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(testGostShouldImit)
{
	// Given
#define LINE(X) \
	0x##X##7, 0x##X##6, 0x##X##5, 0x##X##4, \
	0x##X##3, 0x##X##2, 0x##X##1, 0x##X##0, \
	0x##X##F, 0x##X##E, 0x##X##D, 0x##X##C, \
	0x##X##B, 0x##X##A, 0x##X##9, 0x##X##8
	const vector<uint8_t> text = {
		LINE(0),
		LINE(1),
		LINE(2),
		LINE(3),
		LINE(4),
		LINE(5),
		LINE(6),
		LINE(7),
		LINE(8),
		LINE(9),
		LINE(A),
		LINE(B),
		LINE(C),
		LINE(D),
		LINE(E),
		LINE(F) };
#undef LINE
	const vector<uint8_t> key = {
		0x04, 0x75, 0xF6, 0xE0, 0x50, 0x38, 0xFB, 0xFA,
		0xD2, 0xC7, 0xC3, 0x90, 0xED, 0xB3, 0xCA, 0x3D,
		0x15, 0x47, 0x12, 0x42, 0x91, 0xAE, 0x1E, 0x8A,
		0x2F, 0x79, 0xCD, 0x9E, 0xD2, 0xBC, 0xEF, 0xBD };
	// When
	const auto imit = gost_imit(text, key);
	// Them
	const vector<uint8_t> expected = { 0x54, 0x8F, 0x73, 0x46 };
	BOOST_REQUIRE_EQUAL_COLLECTIONS(imit.begin(), imit.begin() + expected.size(),
					expected.begin(), expected.end());
}
