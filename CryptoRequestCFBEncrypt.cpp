#include "CryptoRequestCFBEncrypt.h"
#include "async-gost.h"
#include "CryptoEngineSlot.h"

using namespace std;

CryptoRequestCFBEncrypt::CryptoRequestCFBEncrypt(const vector<uint8_t> &source,
	const vector<uint8_t> &key, const vector<uint8_t> &init)
	: size(source.size()), index(0), key(key),
	  iv({*reinterpret_cast<const uint32_t *>(&init[0]),
		*reinterpret_cast<const uint32_t *>(&init[4])})
{
	const uint32_t *data_ptr = reinterpret_cast<const uint32_t *>(&source[0]);
	// В массиве data хранится четное количество 32-битных слов (дополнение по барабану)
	data.assign(data_ptr, data_ptr + ((size + 7) & ~7) / sizeof(uint32_t));
}

void CryptoRequestCFBEncrypt::init(CryptoEngineSlot *slot) const
{
	slot->setKey(&key[0]);
}

void CryptoRequestCFBEncrypt::load(CryptoEngineSlot *slot) const
{
	slot->setBlock(iv[0], iv[1]);
}

void CryptoRequestCFBEncrypt::save(CryptoEngineSlot *slot)
{
	slot->xorData(&data[index], &data[index + 1]);
	iv[0] = data[index];
	iv[1] = data[index + 1];
	index += 2;
}

bool CryptoRequestCFBEncrypt::isDone() const
{
	return index == data.size();
}

void CryptoRequestCFBEncrypt::submit()
{
	ContextReply reply;
	reply.data.assign(reinterpret_cast<uint8_t *>(&data[0]),
			reinterpret_cast<uint8_t *>(&data[0]) + size);
	result.set_value(reply);
}
