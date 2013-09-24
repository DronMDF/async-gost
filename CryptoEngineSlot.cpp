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

void CryptoEngineSlot::setA(uint32_t A)
{
	*this->A = A;
}

void CryptoEngineSlot::setB(uint32_t B)
{
	*this->B = B;
}

void CryptoEngineSlot::setBlock(uint32_t A, uint32_t B)
{
	*this->A = A;
	*this->B = B;
}

uint32_t CryptoEngineSlot::xorA(uint32_t A)
{
	*this->A ^= A;
	return *this->A;
}

uint32_t CryptoEngineSlot::xorB(uint32_t B)
{
	*this->B ^= B;
	return *this->B;
}

void CryptoEngineSlot::xorBlock(uint32_t *A, uint32_t *B)
{
	*this->A ^= *A;
	*this->B ^= *B;
	*A = *this->A;
	*B = *this->B;
}
