#pragma once
#include <cstdint>
#include <memory>
#include "CryptoEngineSlot.h"

class CryptoRequest;

class CryptoEngineGeneric
{
public:
	CryptoEngineGeneric();

	CryptoEngineSlot slot;

	void imit();
	void encrypt();

private:
	void expand_tab(const uint8_t sbox[64], uint32_t tab[256], int shift) const;
	void set_sbox(const uint8_t sbox[64]);
	uint32_t step(uint32_t elem1, uint32_t elem2, uint32_t key) const;

	uint32_t A;
	uint32_t B;
	uint32_t key[8];

	uint32_t tab1[256];
	uint32_t tab2[256];
	uint32_t tab3[256];
	uint32_t tab4[256];
};
