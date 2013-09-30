
#pragma once
#include <cstdint>
#include <functional>

class CryptoEngineSlot
{
public:
	CryptoEngineSlot(std::function<void(const void *key)> setkey, uint32_t *A, uint32_t *B);

	void setKey(const void *key) const;
	void setBlock(uint32_t A, uint32_t B) const;
	void xorBlock(uint32_t A, uint32_t B) const;

	void xorData(uint32_t * const A, uint32_t * const B) const;
	void getData(uint32_t * const A, uint32_t * const B) const;

private:
	std::function<void(const void *key)> setkey;
	uint32_t * const A;
	uint32_t * const B;
};
