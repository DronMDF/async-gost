#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "CryptoEngine.h"
#include "CryptoEngineSlot.h"

class CryptoRequest;

class CryptoEngineSSSE3 : public CryptoEngine
{
public:
	CryptoEngineSSSE3();

	void imit() override;
	void encrypt() override;

private:
	typedef unsigned v4si __attribute__ ((vector_size (16)));
	typedef unsigned char v16qi __attribute__ ((vector_size (16)));

	v16qi expand_tab(const uint8_t sbox[64], int li, int hi) const;
	void set_sbox(const uint8_t sbox[64]);
	void set_key(int slot, const void *source_key);
	v4si step(v4si a, v4si b, v4si key) const;

	v4si A;
	v4si B;
	v4si key[8];

	v16qi tab1;
	v16qi tab2;
	v16qi tab3;
	v16qi tab4;
};
