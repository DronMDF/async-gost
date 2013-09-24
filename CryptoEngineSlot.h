
#pragma once
#include <cstdint>

class CryptoEngineSlot
{
public:
	CryptoEngineSlot(uint32_t *key, uint32_t *A, uint32_t *B);

	void setKey(const void *key);
	void setBlock(uint32_t A, uint32_t B);
	// Ксорит контекст и данные, результаты попадают и в контекст и в данные
	void xorBlock(uint32_t *A, uint32_t *B);

private:
	uint32_t * const key;
	uint32_t * const A;
	uint32_t * const B;
};
