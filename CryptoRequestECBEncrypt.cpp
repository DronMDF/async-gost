
#include "CryptoRequestECBEncrypt.h"
#include "async-gost.hpp"
#include "CryptoEngineSlot.h"

using namespace std;

CryptoRequestECBEncrypt::CryptoRequestECBEncrypt(const vector<uint8_t> &source,
	const vector<uint8_t> &key)
	: index(0), key(key)
{
	if (source.size() % 8 != 0) {
		throw runtime_error("Размер данных для простой замены дожен быть выровнен "
			"на границу блока (8 байт)");
	}
	const uint32_t *data_ptr = reinterpret_cast<const uint32_t *>(&source[0]);
	data.assign(data_ptr, data_ptr + source.size() / sizeof(uint32_t));
}

void CryptoRequestECBEncrypt::init(CryptoEngineSlot *slot) const
{
	slot->setKey(&key[0]);
	slot->setBlock(data[0], data[1]);
}

void CryptoRequestECBEncrypt::update(CryptoEngineSlot *slot)
{
	slot->getData(&data[index], &data[index + 1]);
	index += 2;
	slot->setBlock(data[index], data[index + 1]);
}

bool CryptoRequestECBEncrypt::isDone() const
{
	return index == data.size();
}

void CryptoRequestECBEncrypt::submit()
{
	ContextReply reply;
	reply.data.assign(reinterpret_cast<uint8_t *>(&data[0]),
			reinterpret_cast<uint8_t *>(&data[0]) + data.size() * sizeof(uint32_t));
	result.set_value(reply);
}
