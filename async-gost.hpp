
#pragma once
#include <cstdint>
#include <cstddef>
#include <future>
#include <vector>

class ContextReply {
public:
	std::vector<uint8_t> data;
	std::size_t size() const {
		return data.size();
	}
};

void add_crypto_thread();

std::future<ContextReply> async_cfb_encrypt(const std::vector<uint8_t> &data, const std::vector<uint8_t> &key,
	const std::vector<uint8_t> &iv);
