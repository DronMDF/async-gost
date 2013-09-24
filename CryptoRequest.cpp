#include "CryptoRequest.h"
#include <cstring>
#include "async-gost.hpp"
#include "CryptoEngineSlot.h"

using namespace std;

CryptoRequest::CryptoRequest(const vector<uint8_t> &source, const vector<uint8_t> &key,
	      const vector<uint8_t> &init)
	: size(source.size()), index(0), key(key),
	  iv({*reinterpret_cast<const uint32_t *>(&init[0]),
		*reinterpret_cast<const uint32_t *>(&init[4])})
{
	const uint32_t *data_ptr = reinterpret_cast<const uint32_t *>(&source[0]);
	// В массиве data хранится четное количество 32-битных слов
	data.assign(data_ptr, data_ptr + ((size + 7) & ~7) / sizeof(uint32_t));
}

future<ContextReply> CryptoRequest::get_future()
{
	return result.get_future();
}

void CryptoRequest::init(CryptoEngineSlot *slot) const
{
	slot->setKey(&key[0]);
	slot->setBlock(iv[0], iv[1]);
}

void CryptoRequest::update(CryptoEngineSlot *slot)
{
	slot->xorBlock(&data[index], &data[index + 1]);
	index += 2;
}

bool CryptoRequest::isDone() const
{
	return index == data.size();
}

void CryptoRequest::submit()
{
	ContextReply reply;
	reply.data.assign(reinterpret_cast<uint8_t *>(&data[0]),
			reinterpret_cast<uint8_t *>(&data[0]) + size);
	result.set_value(reply);
}
