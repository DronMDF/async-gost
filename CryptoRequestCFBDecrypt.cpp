
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

void CryptoRequestCFBDecrypt::init(CryptoEngineSlot *slot) const
{
	slot->setKey(&key[0]);
	slot->setBlock(iv[0], iv[1]);
}

void CryptoRequestCFBDecrypt::update(CryptoEngineSlot *slot)
{
	const uint32_t A = data[index];
	const uint32_t B = data[index + 1];
	slot->xorData(&data[index], &data[index + 1]);
	slot->setBlock(A, B);
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
