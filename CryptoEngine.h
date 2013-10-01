
#pragma once
#include <vector>

class CryptoEngineSlot;

class CryptoEngine {
public:
	CryptoEngine(const std::vector<CryptoEngineSlot> &slots);

	virtual void imit() = 0;
	virtual void encrypt() = 0;

	const std::vector<CryptoEngineSlot> slots;
};
