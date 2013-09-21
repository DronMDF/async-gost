#pragma once
#include <cstdint>

class GostGenericEngine
{
public:
	GostGenericEngine();

	// Это контекст шифратора - надо подумать как предоставлять его для модификации пользователями
	uint32_t iv[2];
	uint32_t key[8];

	void imit();
	void encrypt();
private:
	void expand_tab(const uint8_t sbox[64], uint32_t tab[256], int shift) const;
	void set_sbox(const uint8_t sbox[64]);
	uint32_t step(uint32_t elem1, uint32_t elem2, uint32_t key) const;

	uint32_t tab1[256];
	uint32_t tab2[256];
	uint32_t tab3[256];
	uint32_t tab4[256];
};
