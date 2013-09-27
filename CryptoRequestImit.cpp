
#include "CryptoRequestImit.h"
#include <cassert>
#include <cstring>
#include "async-gost.h"
#include "CryptoEngineSlot.h"

using namespace std;

CryptoRequestImit::CryptoRequestImit(const vector<uint8_t> &source, const vector<uint8_t> &key)
	: index(0), key(key), imit(8)
{
	const auto size = source.size();
	// В массиве data хранится определенное количество двойных 32-битных слов
	// Здесь нужно аккуратно переписывать данные, потому что нули в буфере имеют значение
	// В отличии от cfb
	const auto data_size = ((size + 7) & ~7) / sizeof(uint32_t);
	assert(data_size % 2 == 0);
	data.resize(max(data_size, 4LU), 0);
	memcpy(&data[0], &source[0], size);
}

void CryptoRequestImit::init(CryptoEngineSlot *slot) const
{
	slot->setKey(&key[0]);
	slot->setBlock(0, 0);
}

void CryptoRequestImit::load(CryptoEngineSlot *slot) const
{
	slot->xorBlock(data[index], data[index + 1]);
}

void CryptoRequestImit::save(CryptoEngineSlot *slot)
{
	index += 2;
	if (index == data.size()) {
		uint32_t *imit_ptr = reinterpret_cast<uint32_t *>(&imit[0]);
		slot->getData(imit_ptr, imit_ptr + 1);
	}
}

bool CryptoRequestImit::isDone() const
{
	return index == data.size();
}

void CryptoRequestImit::submit()
{
	ContextReply reply;
	reply.data = imit;
	result.set_value(reply);
}
