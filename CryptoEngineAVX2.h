#pragma once
#include <array>
#include <cstdint>
#include "CryptoEngine.h"
#include "CryptoEngineSlot.h"

class CryptoRequest;

class CryptoEngineAVX2 : public CryptoEngine
{
public:
	CryptoEngineAVX2();
	std::vector<CryptoEngineSlot> getSlots() override;

	void imit() override;
	void encrypt() override;

private:
	typedef unsigned v8si __attribute__ ((vector_size (32)));
	typedef unsigned char v32qi __attribute__ ((vector_size (32)));

	v32qi expand_tab(const uint8_t sbox[64], int li, int hi) const;
	void set_sbox(const uint8_t sbox[64]);
	void set_key(int slot, const void *source_key);
	v8si step(v8si a, v8si b, v8si key) const;

	v8si A __attribute__((aligned(32)));
	v8si B __attribute__((aligned(32)));
	v8si key[8] __attribute__((aligned(32)));

	v32qi tab1 __attribute__((aligned(32)));
	v32qi tab2 __attribute__((aligned(32)));
	v32qi tab3 __attribute__((aligned(32)));
	v32qi tab4 __attribute__((aligned(32)));
};
