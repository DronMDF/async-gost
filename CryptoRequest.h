
#pragma once
#include <future>

class ContextReply;
class CryptoEngineSlot;

class CryptoRequest
{
public:
	std::future<ContextReply> get_future();

	virtual void init(CryptoEngineSlot *slot) const = 0;
	virtual void update(CryptoEngineSlot *slot) = 0;

	virtual bool isDone() const = 0;
	virtual void submit() = 0;

protected:
	std::promise<ContextReply> result;
};
