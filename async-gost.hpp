
#pragma once
#include <cstdint>
#include <cstddef>
#include <future>
#include <vector>

class ContextReply {
public:
	std::vector<uint8_t> data;
};

void add_crypto_thread();

std::future<ContextReply> async_cfb_encrypt(const std::vector<uint8_t> &data,
	const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
std::future<ContextReply> async_cfb_decrypt(const std::vector<uint8_t> &data,
	const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
std::future<ContextReply> async_ecb_encrypt(const std::vector<uint8_t> &data,
	const std::vector<uint8_t> &key);
