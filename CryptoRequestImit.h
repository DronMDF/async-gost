
#pragma once
#include <vector>
#include "CryptoRequest.h"

class CryptoRequestImit : public CryptoRequest
{
public:
	CryptoRequestImit(const std::vector<uint8_t> &source, const std::vector<uint8_t> &key);

	void init(CryptoEngineSlot *slot) const override;
	void load(CryptoEngineSlot *slot) const override;
	void save(CryptoEngineSlot *slot) override;

	bool isDone() const override;
	void submit() override;

private:
	size_t index;
	std::vector<uint8_t> key;
	std::vector<uint32_t> data;
	std::vector<uint8_t> imit;
};
