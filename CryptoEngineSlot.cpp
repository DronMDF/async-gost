#include "CryptoEngineSlot.h"
#include <cstring>

using namespace std;

CryptoEngineSlot::CryptoEngineSlot(function<void(const void *key)> setkey, uint32_t *A, uint32_t *B)
	: setkey(setkey), A(A), B(B)
{

}

void CryptoEngineSlot::setKey(const void *key) const
{
	setkey(key);
}

void CryptoEngineSlot::setBlock(uint32_t A, uint32_t B) const
{
	*this->A = A;
	*this->B = B;
}

void CryptoEngineSlot::xorBlock(uint32_t A, uint32_t B) const
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
