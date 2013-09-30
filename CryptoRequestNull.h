
#pragma once
#include "CryptoRequest.h"

class CryptoRequestNull : public CryptoRequest
{
public:
	bool isDone() const override;
};
