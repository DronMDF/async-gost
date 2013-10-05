
#include "CryptoEngineSSSE3.h"

#include <cstring>
#include <utility>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "CpuSupport.h"
#include "CryptoRequest.h"

using namespace std;
using namespace std::placeholders;

CryptoEngineSSSE3::CryptoEngineSSSE3()
	: slots({
		CryptoEngineSlot(bind(&CryptoEngineSSSE3::set_key, this, 0, _1), &reinterpret_cast<uint32_t *>(&A)[0], &reinterpret_cast<uint32_t *>(&B)[0]),
		CryptoEngineSlot(bind(&CryptoEngineSSSE3::set_key, this, 1, _1), &reinterpret_cast<uint32_t *>(&A)[1], &reinterpret_cast<uint32_t *>(&B)[1]),
		CryptoEngineSlot(bind(&CryptoEngineSSSE3::set_key, this, 2, _1), &reinterpret_cast<uint32_t *>(&A)[2], &reinterpret_cast<uint32_t *>(&B)[2]),
		CryptoEngineSlot(bind(&CryptoEngineSSSE3::set_key, this, 3, _1), &reinterpret_cast<uint32_t *>(&A)[3], &reinterpret_cast<uint32_t *>(&B)[3]),
	})
{
	const uint8_t FapsiSubst[] = {
		0xc4, 0xed, 0x83, 0xc9, 0x92, 0x98, 0xfe, 0x6b,
		0xff, 0xbe, 0x65, 0x5c, 0xe5, 0x2c, 0xb9, 0x20,
		0x89, 0x57, 0x16, 0xb3, 0x11, 0xf3, 0x98, 0x06,
		0x30, 0x79, 0xc0, 0x97, 0xa8, 0x1a, 0x5d, 0xd5,
		0x2e, 0x01, 0xda, 0x34, 0x73, 0xd5, 0x3b, 0xe8,
		0x4b, 0xc2, 0x77, 0x7e, 0xdc, 0x64, 0xac, 0xaf,
		0x6d, 0xa6, 0x02, 0xf1, 0x07, 0x4f, 0xe1, 0x4a,
		0xba, 0x30, 0x2f, 0x12, 0x56, 0x8b, 0x44, 0x8d
	};

	set_sbox(FapsiSubst);
}

CryptoEngineSSSE3::v16qi CryptoEngineSSSE3::expand_tab(const uint8_t sbox[64], int li, int hi) const
{
	v16qi tab;
	for (int i = 0; i < 16; i++) {
		reinterpret_cast<uint8_t *>(&tab)[i] =
			(sbox[i * 4 + li] & 0x0f) | (sbox[i * 4 + hi] & 0xf0);
	}
	return tab;
}

void CryptoEngineSSSE3::set_sbox(const uint8_t sbox[64])
{
	tab1 = expand_tab(sbox, 0, 1);
	tab2 = expand_tab(sbox, 1, 0);
	tab3 = expand_tab(sbox, 2, 3);
	tab4 = expand_tab(sbox, 3, 2);
}

void CryptoEngineSSSE3::set_key(int slot, const void *source_key)
{
	assert(slot < 4);
	for (int i = 0; i < 8; i++) {
		reinterpret_cast<uint32_t *>(&key[i])[slot] =
			reinterpret_cast<const uint32_t *>(source_key)[i];
	}
}

CryptoEngineSSSE3::v4si CryptoEngineSSSE3::step(v4si a, v4si b, v4si key) const
{
	static const v4si lo_mask = { 0x000f000f, 0x000f000f, 0x000f000f, 0x000f000f };
	static const v4si hi_mask = { 0x0f000f00, 0x0f000f00, 0x0f000f00, 0x0f000f00 };

	static const v16qi b0_mask = { 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00,
					0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00 };
	static const v16qi b1_mask = { 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00,
					0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00 };
	static const v16qi b2_mask = { 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0,
					0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0 };
	static const v16qi b3_mask = { 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f,
					0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f };

	const v4si lo = key + b;
	const v4si hi = __builtin_ia32_psrldi128(lo, 4);
	const v4si loc = (lo & lo_mask) | (hi & hi_mask);
	const v4si hic = (lo & hi_mask) | (hi & lo_mask);
	const v16qi b0 = __builtin_ia32_pshufb128(tab1, loc) & b0_mask; // 0x????1??0
	const v16qi b1 = __builtin_ia32_pshufb128(tab2, hic) & b1_mask; // 0x?????10?
	const v16qi b2 = __builtin_ia32_pshufb128(tab3, loc) & b2_mask; // 0x3??2????
	const v16qi b3 = __builtin_ia32_pshufb128(tab4, hic) & b3_mask; // 0x?32?????
	const v4si i1 = b0 | b1 | b2 | b3;
	return a ^ (__builtin_ia32_pslldi128(i1, 11) | __builtin_ia32_psrldi128(i1, 21));
}

void CryptoEngineSSSE3::imit()
{
	for (int i = 0; i < 2; i++) {
		B = step(B, A, key[0]);
		A = step(A, B, key[1]);
		B = step(B, A, key[2]);
		A = step(A, B, key[3]);
		B = step(B, A, key[4]);
		A = step(A, B, key[5]);
		B = step(B, A, key[6]);
		A = step(A, B, key[7]);
	}
}

void CryptoEngineSSSE3::encrypt()
{
	swap(A, B);

	for (int i = 0; i < 3; i++) {
		A = step(A, B, key[0]);
		B = step(B, A, key[1]);
		A = step(A, B, key[2]);
		B = step(B, A, key[3]);
		A = step(A, B, key[4]);
		B = step(B, A, key[5]);
		A = step(A, B, key[6]);
		B = step(B, A, key[7]);
	}

	A = step(A, B, key[7]);
	B = step(B, A, key[6]);
	A = step(A, B, key[5]);
	B = step(B, A, key[4]);
	A = step(A, B, key[3]);
	B = step(B, A, key[2]);
	A = step(A, B, key[1]);
	B = step(B, A, key[0]);
}

unsigned CryptoEngineSSSE3::getSlotCount() const
{
	return slots.size();
}

const CryptoEngineSlot *CryptoEngineSSSE3::getSlot(unsigned slot) const
{
	assert(slot < 4);
	return &slots[slot];
}

BOOST_AUTO_TEST_SUITE(suiteCryptoEngineSSSE3)

void CUSTOM_REQUIRE_ENCRYPT(const vector<uint8_t> &key, uint32_t A, uint32_t B, uint32_t eA, uint32_t eB)
{
	shared_ptr<CryptoEngine> engine = make_shared<CryptoEngineSSSE3>();
	for (unsigned s = 0; s < engine->getSlotCount(); s++) {
		engine->getSlot(s)->setKey(&key[0]);
		engine->getSlot(s)->setBlock(A, B);
		engine->encrypt();
		uint32_t rA, rB;
		engine->getSlot(s)->getData(&rA, &rB);
		BOOST_REQUIRE_EQUAL(rA, eA);
		BOOST_REQUIRE_EQUAL(rB, eB);
	}
}

BOOST_AUTO_TEST_CASE(encryptShouldBeCorrect)
{
	if (!cpu_support_ssse3()) {
		cout << "CPU not support ssse3, test skipped" << endl;
		return;
	}

	const vector<uint8_t> key01 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	CUSTOM_REQUIRE_ENCRYPT(key01, 0xCCCCCCCC, 0x33333333, 0xF5FE5211, 0x17E8D02E);
	CUSTOM_REQUIRE_ENCRYPT(key01, 0x33333333, 0xCCCCCCCC, 0x6390ED97, 0x3A962C89);

	const vector<uint8_t> key02 = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	CUSTOM_REQUIRE_ENCRYPT(key02, 0xCCCCCCCC, 0x33333333, 0x2A78B7E0, 0x800A0268);
	CUSTOM_REQUIRE_ENCRYPT(key02, 0x33333333, 0xCCCCCCCC, 0x462DA336, 0xEAB90129);

	const vector<uint8_t> key03 = {
		0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa,
		0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa,
		0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa,
		0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa };
	CUSTOM_REQUIRE_ENCRYPT(key03, 0xCCCCCCCC, 0x33333333, 0x8BB8CF97, 0x533CDA6B);
	CUSTOM_REQUIRE_ENCRYPT(key03, 0x33333333, 0xCCCCCCCC, 0xBE407AB5, 0x5C055B4F);

	const vector<uint8_t> key04 = {
		0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
		0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
		0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
		0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55 };
	CUSTOM_REQUIRE_ENCRYPT(key04, 0xCCCCCCCC, 0x33333333, 0x895A9742, 0x02DB134C);
	CUSTOM_REQUIRE_ENCRYPT(key04, 0x33333333, 0xCCCCCCCC, 0xDAA70325, 0xB95DDC39);

	const vector<uint8_t> key05 = {
		0x04, 0x75, 0xF6, 0xE0, 0x50, 0x38, 0xFB, 0xFA,
		0xD2, 0xC7, 0xC3, 0x90, 0xED, 0xB3, 0xCA, 0x3D,
		0x15, 0x47, 0x12, 0x42, 0x91, 0xAE, 0x1E, 0x8A,
		0x2F, 0x79, 0xCD, 0x9E, 0xD2, 0xBC, 0xEF, 0xBD };
	CUSTOM_REQUIRE_ENCRYPT(key05, 0xCCCCCCCC, 0x33333333, 0x401EBED9, 0x56F5D77D);
	CUSTOM_REQUIRE_ENCRYPT(key05, 0x33333333, 0xCCCCCCCC, 0x4E790503, 0x73FE0118);
}

BOOST_AUTO_TEST_SUITE_END()
