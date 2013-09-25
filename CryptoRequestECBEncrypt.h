
#pragma once
#include "CryptoRequest.h"
#include <vector>

class CryptoRequestECBEncrypt : public CryptoRequest
{
public:
	CryptoRequestECBEncrypt(const std::vector<uint8_t> &source, const std::vector<uint8_t> &key);

	void init(CryptoEngineSlot *slot) const override;
	void update(CryptoEngineSlot *slot) override;

	bool isDone() const override;
	void submit() override;

private:
	size_t index;
	std::vector<uint8_t> key;
	std::vector<uint32_t> data;
};
