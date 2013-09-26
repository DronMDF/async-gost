#include "CryptoEngineSlot.h"
#include <cstring>

CryptoEngineSlot::CryptoEngineSlot(uint32_t *key, uint32_t *A, uint32_t *B)
	: key(key), A(A), B(B)
{

}

void CryptoEngineSlot::setKey(const void *key)
{
	memcpy(this->key, key, 32);
}

void CryptoEngineSlot::setBlock(uint32_t A, uint32_t B)
{
	*this->A = A;
	*this->B = B;
}

void CryptoEngineSlot::xorBlock(uint32_t A, uint32_t B)
{
	*this->A ^= A;
	*this->B ^= B;
}

void CryptoEngineSlot::xorData(uint32_t * const A, uint32_t * const B) const
{
	*A ^= *this->A;
	*B ^= *this->B;
}

void CryptoEngineSlot::getData(uint32_t * const A, uint32_t * const B) const
{
	*A = *this->A;
	*B = *this->B;
}
