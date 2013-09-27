
#include "CryptoRequestImit.h"
#include "async-gost.h"
#include "CryptoEngineSlot.h"

using namespace std;

CryptoRequestImit::CryptoRequestImit(const vector<uint8_t> &source, const vector<uint8_t> &key)
	: index(0), key(key), imit(8)
{
	const size_t size = source.size();
	const uint32_t *data_ptr = reinterpret_cast<const uint32_t *>(&source[0]);
	// В массиве data хранится четное количество 32-битных слов (данные дополняются нулями)
	data.assign(data_ptr, data_ptr + ((size + 7) & ~7) / sizeof(uint32_t));
	while (data.size() < 4) {
		data.push_back(0);
	}
}

void CryptoRequestImit::init(CryptoEngineSlot *slot) const
{
	slot->setKey(&key[0]);
	slot->setBlock(data[0], data[1]);
}

void CryptoRequestImit::update(CryptoEngineSlot *slot)
{
	index += 2;
	if (index == data.size()) {
		uint32_t *imit_ptr = reinterpret_cast<uint32_t *>(&imit[0]);
		slot->getData(imit_ptr, imit_ptr + 1);
		return;
	}
	slot->xorBlock(data[index], data[index + 1]);
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
