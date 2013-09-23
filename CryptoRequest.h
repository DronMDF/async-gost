
#pragma once
#include <cstdint>
#include <cstddef>
#include <future>
#include <vector>

class ContextReply;

class CryptoRequest
{
public:
	CryptoRequest(const std::vector<uint8_t> &data, const std::vector<uint8_t> &key,
		      const std::vector<uint8_t> &iv);

	std::future<ContextReply> get_future();
	std::vector<uint8_t> getKey() const;
	std::vector<uint8_t> getData() const;
	void setData(const std::vector<uint8_t> &data);
	bool isDone() const;
	void submit();

private:
	std::promise<ContextReply> result;
	std::vector<uint8_t> data;
	std::vector<uint8_t> key;
	std::vector<uint8_t> iv;
	size_t index;
};
