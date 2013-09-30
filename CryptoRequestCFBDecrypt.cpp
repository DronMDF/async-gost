
#include "CryptoRequestCFBDecrypt.h"
#include "async-gost.h"
#include "CryptoEngineSlot.h"

using namespace std;

CryptoRequestCFBDecrypt::CryptoRequestCFBDecrypt(const vector<uint8_t> &source,
	const vector<uint8_t> &key, const vector<uint8_t> &init)
	: size(source.size()), index(0), key(key),
	  iv({*reinterpret_cast<const uint32_t *>(&init[0]),
		*reinterpret_cast<const uint32_t *>(&init[4])})
{
	const uint32_t *data_ptr = reinterpret_cast<const uint32_t *>(&source[0]);
	// В массиве data хранится четное количество 32-битных слов (дополнение по барабану)
	data.assign(data_ptr, data_ptr + ((size + 7) & ~7) / sizeof(uint32_t));
}

void CryptoRequestCFBDecrypt::init(const CryptoEngineSlot *slot) const
{
	slot->setKey(&key[0]);
}

void CryptoRequestCFBDecrypt::load(const CryptoEngineSlot *slot) const
{
	slot->setBlock(iv[0], iv[1]);
}

void CryptoRequestCFBDecrypt::save(const CryptoEngineSlot *slot)
{
	iv[0] = data[index];
	iv[1] = data[index + 1];
	slot->xorData(&data[index], &data[index + 1]);
	index += 2;
}

bool CryptoRequestCFBDecrypt::isDone() const
{
	return index == data.size();
}

void CryptoRequestCFBDecrypt::submit()
{
	ContextReply reply;
	reply.data.assign(reinterpret_cast<uint8_t *>(&data[0]),
			reinterpret_cast<uint8_t *>(&data[0]) + size);
	result.set_value(reply);
}
