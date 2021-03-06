
#include "CryptoEngineSSSE3.h"

#include <cassert>
#include <cstring>
#include <utility>
#include <vector>
#include "CpuSupport.h"
#include "CryptoRequest.h"
#include "upp11.h"

using namespace std;
using namespace std::placeholders;

CryptoEngineSSSE3::CryptoEngineSSSE3()
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
	v16qi tab __attribute__((aligned(16)));
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
	const v4si lo_mask = { 0x000f000f, 0x000f000f, 0x000f000f, 0x000f000f };
	const v4si hi_mask = { 0x0f000f00, 0x0f000f00, 0x0f000f00, 0x0f000f00 };

	const v16qi b0_mask = { 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00,
					0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00 };
	const v16qi b1_mask = { 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00,
					0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00 };
	const v16qi b2_mask = { 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0,
					0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0 };
	const v16qi b3_mask = { 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f,
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

vector<CryptoEngineSlot> CryptoEngineSSSE3::getSlots()
{
	return {
		CryptoEngineSlot(bind(&CryptoEngineSSSE3::set_key, this, 0, _1), &reinterpret_cast<uint32_t *>(&A)[0], &reinterpret_cast<uint32_t *>(&B)[0]),
		CryptoEngineSlot(bind(&CryptoEngineSSSE3::set_key, this, 1, _1), &reinterpret_cast<uint32_t *>(&A)[1], &reinterpret_cast<uint32_t *>(&B)[1]),
		CryptoEngineSlot(bind(&CryptoEngineSSSE3::set_key, this, 2, _1), &reinterpret_cast<uint32_t *>(&A)[2], &reinterpret_cast<uint32_t *>(&B)[2]),
		CryptoEngineSlot(bind(&CryptoEngineSSSE3::set_key, this, 3, _1), &reinterpret_cast<uint32_t *>(&A)[3], &reinterpret_cast<uint32_t *>(&B)[3]),
	};
}

UP_SUITE_BEGIN(suiteCryptoEngineSSSE3)

static const vector<uint8_t> key01 = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static const vector<uint8_t> key02 = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const vector<uint8_t> key03 = {
	0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa,
	0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa,
	0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa,
	0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa };
static const vector<uint8_t> key04 = {
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
	0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55 };
static const vector<uint8_t> key05 = {
	0x04, 0x75, 0xF6, 0xE0, 0x50, 0x38, 0xFB, 0xFA,
	0xD2, 0xC7, 0xC3, 0x90, 0xED, 0xB3, 0xCA, 0x3D,
	0x15, 0x47, 0x12, 0x42, 0x91, 0xAE, 0x1E, 0x8A,
	0x2F, 0x79, 0xCD, 0x9E, 0xD2, 0xBC, 0xEF, 0xBD };

static const auto encrypt_params = {
	make_tuple(key01, 0xCCCCCCCCU, 0x33333333U, 0xF5FE5211U, 0x17E8D02EU),
	make_tuple(key01, 0x33333333U, 0xCCCCCCCCU, 0x6390ED97U, 0x3A962C89U),
	make_tuple(key02, 0xCCCCCCCCU, 0x33333333U, 0x2A78B7E0U, 0x800A0268U),
	make_tuple(key02, 0x33333333U, 0xCCCCCCCCU, 0x462DA336U, 0xEAB90129U),
	make_tuple(key03, 0xCCCCCCCCU, 0x33333333U, 0x8BB8CF97U, 0x533CDA6BU),
	make_tuple(key03, 0x33333333U, 0xCCCCCCCCU, 0xBE407AB5U, 0x5C055B4FU),
	make_tuple(key04, 0xCCCCCCCCU, 0x33333333U, 0x895A9742U, 0x02DB134CU),
	make_tuple(key04, 0x33333333U, 0xCCCCCCCCU, 0xDAA70325U, 0xB95DDC39U),
	make_tuple(key05, 0xCCCCCCCCU, 0x33333333U, 0x401EBED9U, 0x56F5D77DU),
	make_tuple(key05, 0x33333333U, 0xCCCCCCCCU, 0x4E790503U, 0x73FE0118U),
};

UP_PARAMETRIZED_TEST(encryptShouldBeCorrect, encrypt_params)
{
	if (!cpu_support_ssse3()) {
		cout << "CPU not support ssse3, test skipped" << endl;
		return;
	}

	const auto key = get<0>(encrypt_params);
	const auto A = get<1>(encrypt_params);
	const auto B = get<2>(encrypt_params);
	const auto eA = get<3>(encrypt_params);
	const auto eB = get<4>(encrypt_params);

	shared_ptr<CryptoEngine> engine = make_shared<CryptoEngineSSSE3>();
	const auto slots = engine->getSlots();
	for (auto slot: slots) {
		slot.setKey(&key[0]);
		slot.setBlock(A, B);
		engine->encrypt();
		uint32_t rA, rB;
		slot.getData(&rA, &rB);
		UP_ASSERT_EQUAL(rA, eA);
		UP_ASSERT_EQUAL(rB, eB);
	}
}

UP_SUITE_END()
