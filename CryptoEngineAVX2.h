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

	unsigned getSlotCount() const override;
	const CryptoEngineSlot *getSlot(unsigned slot) const override;

	void imit() override;
	void encrypt() override;

private:
	typedef unsigned v8si __attribute__ ((vector_size (32)));
	typedef unsigned char v32qi __attribute__ ((vector_size (32)));

	v32qi expand_tab(const uint8_t sbox[64], int li, int hi) const;
	void set_sbox(const uint8_t sbox[64]);
	void set_key(int slot, const void *source_key);
	v8si step(v8si a, v8si b, v8si key) const;

	v8si A;
	v8si B;
	v8si key[8];

	v32qi tab1;
	v32qi tab2;
	v32qi tab3;
	v32qi tab4;

	std::array<CryptoEngineSlot, 4> slots;
};
