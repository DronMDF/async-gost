
#pragma once
#include <vector>

class CryptoEngineSlot;

class CryptoEngine {
public:
	virtual unsigned getSlotCount() const = 0;
	virtual const CryptoEngineSlot *getSlot(unsigned slot) const = 0;

	virtual void imit() = 0;
	virtual void encrypt() = 0;
};
