
#pragma once
#include <cstdint>
#include <cstddef>
#include <future>
#include <vector>

class ContextReply {
public:
	std::vector<uint8_t> data;
};

void crypto_self_test();

enum crypto_engine_t {
	CRYPTO_ENGINE_ENCRYPT_GENERIC,
	CRYPTO_ENGINE_IMIT_GENERIC,
};

void add_crypto_thread(crypto_engine_t type);

std::future<ContextReply> async_cfb_encrypt(const std::vector<uint8_t> &data,
	const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
std::future<ContextReply> async_cfb_decrypt(const std::vector<uint8_t> &data,
	const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
std::future<ContextReply> async_ecb_encrypt(const std::vector<uint8_t> &data,
	const std::vector<uint8_t> &key);
std::future<ContextReply> async_imit(const std::vector<uint8_t> &data,
	const std::vector<uint8_t> &key);

