
#include "CryptoEngineGeneric.h"

#include <cstring>
#include <utility>
#include "CryptoRequest.h"

using namespace std;

CryptoEngineGeneric::CryptoEngineGeneric()
	: slot(&key[0], &iv[0], &iv[1])
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

void CryptoEngineGeneric::expand_tab(const uint8_t sbox[64], uint32_t tab[256], int shift) const
{
	for (int i = 0; i < 256; i++) {
		tab[i] = (sbox[(i / 16) * 4 + shift] & 0xf0) + (sbox[(i % 16) * 4 + shift] & 0x0f);
		tab[i] <<= shift * 8;
		tab[i] = (tab[i] << 11) | ((tab[i] >> 21) & 0x7ff);
	}
}

void CryptoEngineGeneric::set_sbox(const uint8_t sbox[64])
{
	expand_tab(sbox, tab1, 0);
	expand_tab(sbox, tab2, 1);
	expand_tab(sbox, tab3, 2);
	expand_tab(sbox, tab4, 3);
}

uint32_t CryptoEngineGeneric::step(uint32_t elem1, uint32_t elem2, uint32_t key) const
{
	const uint32_t tmp = key + elem2;
	return elem1 ^ tab1[tmp & 0xff] ^ tab2[(tmp >> 8) & 0xff] ^ tab3[(tmp >> 16) & 0xff] ^
			tab4[(tmp >> 24) & 0xff];
}

void CryptoEngineGeneric::imit()
{
	for (int i = 0; i < 2; i++) {
		iv[1] = step(iv[1], iv[0], key[0]);
		iv[0] = step(iv[0], iv[1], key[1]);
		iv[1] = step(iv[1], iv[0], key[2]);
		iv[0] = step(iv[0], iv[1], key[3]);
		iv[1] = step(iv[1], iv[0], key[4]);
		iv[0] = step(iv[0], iv[1], key[5]);
		iv[1] = step(iv[1], iv[0], key[6]);
		iv[0] = step(iv[0], iv[1], key[7]);
	}
}

void CryptoEngineGeneric::encrypt()
{
	swap(iv[0], iv[1]);

	for (int i = 0; i < 3; i++) {
		iv[0] = step(iv[0], iv[1], key[0]);
		iv[1] = step(iv[1], iv[0], key[1]);
		iv[0] = step(iv[0], iv[1], key[2]);
		iv[1] = step(iv[1], iv[0], key[3]);
		iv[0] = step(iv[0], iv[1], key[4]);
		iv[1] = step(iv[1], iv[0], key[5]);
		iv[0] = step(iv[0], iv[1], key[6]);
		iv[1] = step(iv[1], iv[0], key[7]);
	}

	iv[0] = step(iv[0], iv[1], key[7]);
	iv[1] = step(iv[1], iv[0], key[6]);
	iv[0] = step(iv[0], iv[1], key[5]);
	iv[1] = step(iv[1], iv[0], key[4]);
	iv[0] = step(iv[0], iv[1], key[3]);
	iv[1] = step(iv[1], iv[0], key[2]);
	iv[0] = step(iv[0], iv[1], key[1]);
	iv[1] = step(iv[1], iv[0], key[0]);
}
