#pragma once
#include "CryptoRequest.h"
#include <vector>

class CryptoRequestCFBEncrypt : public CryptoRequest
{
public:
	CryptoRequestCFBEncrypt(const std::vector<uint8_t> &source,
		const std::vector<uint8_t> &key, const std::vector<uint8_t> &init);

	void init(CryptoEngineSlot *slot) const override;
	void load(CryptoEngineSlot *slot) const override;
	void save(CryptoEngineSlot *slot) override;

	bool isDone() const override;
	void submit() override;

private:
	size_t size;
	size_t index;
	std::vector<uint8_t> key;
	std::vector<uint32_t> iv;
	std::vector<uint32_t> data;
};
