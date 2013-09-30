
#pragma once
#include <future>

class ContextReply;
class CryptoEngineSlot;

class CryptoRequest
{
public:
	std::future<ContextReply> get_future();

	virtual void init(const CryptoEngineSlot *slot) const;
	virtual void load(const CryptoEngineSlot *slot) const;
	virtual void save(const CryptoEngineSlot *slot);

	virtual bool isDone() const = 0;
	virtual void submit();

protected:
	std::promise<ContextReply> result;
};
