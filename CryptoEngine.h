
#pragma once
#include <vector>

class CryptoEngineSlot;

class CryptoEngine {
public:
	virtual std::vector<CryptoEngineSlot> getSlots() = 0;

	virtual void imit() = 0;
	virtual void encrypt() = 0;
};
