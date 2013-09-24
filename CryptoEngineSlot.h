
#pragma once
#include <cstdint>

class CryptoEngineSlot
{
public:
	CryptoEngineSlot(uint32_t *key, uint32_t *A, uint32_t *B);

	void setKey(const void *key);
	void setA(uint32_t A);
	void setB(uint32_t B);
	uint32_t xorA(uint32_t A);
	uint32_t xorB(uint32_t B);

	void setBlock(uint32_t A, uint32_t B);
	void xorBlock(uint32_t *A, uint32_t *B);

private:
	uint32_t * const key;
	uint32_t * const A;
	uint32_t * const B;
};
