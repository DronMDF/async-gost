
#pragma once
#include <cstdint>
#include <cstddef>
#include <future>
#include <vector>

class ContextReply;
class CryptoEngineSlot;

class CryptoRequest
{
public:
	CryptoRequest(const std::vector<uint8_t> &data, const std::vector<uint8_t> &key,
		      const std::vector<uint8_t> &iv);

	std::future<ContextReply> get_future();

	void init(CryptoEngineSlot *slot) const;
	void update(CryptoEngineSlot *slot);

	bool isDone() const;
	void submit();

private:
	std::promise<ContextReply> result;
	size_t size;
	size_t index;
	std::vector<uint8_t> key;
	std::vector<uint32_t> iv;
	std::vector<uint32_t> data;
};
