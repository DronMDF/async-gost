
#define _POSIX_C_SOURCE 200112L

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef uint8_t gostiv_t[8];
typedef uint32_t gostkey_t[8];

// низкоуровневе функции ГОСТ
static __inline__
uint32_t gost_step (const uint32_t elem1, const uint32_t elem2, const uint32_t key, const uint32_t *sbox)
{
	uint32_t tmp = key + elem2;

	return 	elem1 ^
		sbox[tmp & 0xff] ^
		sbox[256 + ((tmp >> 8) & 0xff)] ^
		sbox[512 + ((tmp >> 16) & 0xff)] ^
		sbox[768 + ((tmp >> 24) & 0xff)];
}

static __inline__
void gost_encrypt_block (const uint32_t *in, uint32_t *out, const uint32_t key[8], const uint32_t *sbox)
{
	uint32_t a = in[1];
	uint32_t b = in[0];
	int i;

	for (i = 0; i < 3; i++) {
		a = gost_step(a, b, key[0], sbox);
		b = gost_step(b, a, key[1], sbox);
		a = gost_step(a, b, key[2], sbox);
		b = gost_step(b, a, key[3], sbox);
		a = gost_step(a, b, key[4], sbox);
		b = gost_step(b, a, key[5], sbox);
		a = gost_step(a, b, key[6], sbox);
		b = gost_step(b, a, key[7], sbox);
	}

	a = gost_step(a, b, key[7], sbox);
	b = gost_step(b, a, key[6], sbox);
	a = gost_step(a, b, key[5], sbox);
	b = gost_step(b, a, key[4], sbox);
	a = gost_step(a, b, key[3], sbox);
	b = gost_step(b, a, key[2], sbox);
	a = gost_step(a, b, key[1], sbox);
	b = gost_step(b, a, key[0], sbox);

	out[1] = b;
	out[0] = a;
}

static __inline__
void gost_imit_block (const uint32_t *in, uint32_t *out, const uint32_t key[8], const uint32_t *sbox)
{
	uint32_t a = in[1];
	uint32_t b = in[0];
	int i;

	for (i = 0; i < 2; i++) {
		a = gost_step(a, b, key[0], sbox);
		b = gost_step(b, a, key[1], sbox);
		a = gost_step(a, b, key[2], sbox);
		b = gost_step(b, a, key[3], sbox);
		a = gost_step(a, b, key[4], sbox);
		b = gost_step(b, a, key[5], sbox);
		a = gost_step(a, b, key[6], sbox);
		b = gost_step(b, a, key[7], sbox);
	}

	out[1] = a;
	out[0] = b;
}

void gost_encrypt_cfb(const char *in, char *out, size_t size, 
		      const gostkey_t key, gostiv_t iv, uint32_t *n, const uint32_t *tab) 
{
	if (size < 16 - *n) {
		/* нет смысла извращаться */
		while (size--) {
			if (!(*n)) {
				gost_encrypt_block((int *)iv, (int *)iv,
						(const int *)key, tab);
			}

			iv[*n] = *(out++) = iv[*n] ^ *(in++);
			(*n)++;
			(*n) &= 7;
		}
	} else {
		/* есть хотя бы один полный блок */
		if (*n) {
			size -= 8 - *n;
			while (*n != 8) {
				iv[*n] = *(out++) = iv[*n] ^ *(in++);
				(*n)++;
			}
			*n = 0;
		}
		while (size & (~7)) {
			gost_encrypt_block((int *)iv, (int *)iv,
					(const int *)key, tab);
			((int *)iv)[0] = ((int *)out)[0] = ((int *)iv)[0] ^
				((int *)in)[0];
			((int *)iv)[1] = ((int *)out)[1] = ((int *)iv)[1] ^
				((int *)in)[1];
			size -= 8;
			in += 8;
			out += 8;
		}
		if (size) {
			gost_encrypt_block((int *)iv, (int *)iv, (const int *)key, tab);
			while (size--) {
				iv[*n] = *(out++) = iv[*n] ^ *(in++);
				(*n)++;
			}
		}
	}
}

void gost_imit_long(const char *buf, size_t size, const gostkey_t key, const uint32_t *tab, uint32_t rv[2]) 
{
	int n = 16;
	rv[0] = rv[1] = 0;

	char *result = (char *)rv;
	for (size_t i = 0; i < size; i++) {
		if (n == 8) {
			gost_imit_block((int *)result, (int *)result, (const int *)key, tab);
		}
		n &= 7;
		result[n++] ^= buf[i];
	}
	gost_imit_block((int *)result, (int *)result, (const int *)key, tab);
}

void expand_tab(const char sbox[64], uint32_t tab[256], int shift) 
{
	for (int i = 0; i < 256; i++) {
		tab[i] = (sbox[(i / 16) * 4 + shift] & 0xf0) 
			+ (sbox[(i % 16) * 4 + shift] & 0x0f);
		tab[i] <<= shift * 8;
		tab[i] = (tab[i] << 11) | ((tab[i] >> 21) & 0x7ff);
	}
}

void gost_set_sbox(const char sbox[64], uint32_t *tab) 
{
	expand_tab(sbox, tab, 0);
	expand_tab(sbox, tab + 256, 1);
	expand_tab(sbox, tab + 512, 2);
	expand_tab(sbox, tab + 768, 3);
}

static const char FapsiSubst[] = {
	0xc4, 0xed, 0x83, 0xc9, 0x92, 0x98, 0xfe, 0x6b,
	0xff, 0xbe, 0x65, 0x5c, 0xe5, 0x2c, 0xb9, 0x20,
	0x89, 0x57, 0x16, 0xb3, 0x11, 0xf3, 0x98, 0x06,
	0x30, 0x79, 0xc0, 0x97, 0xa8, 0x1a, 0x5d, 0xd5,
	0x2e, 0x01, 0xda, 0x34, 0x73, 0xd5, 0x3b, 0xe8,
	0x4b, 0xc2, 0x77, 0x7e, 0xdc, 0x64, 0xac, 0xaf,
	0x6d, 0xa6, 0x02, 0xf1, 0x07, 0x4f, 0xe1, 0x4a,
	0xba, 0x30, 0x2f, 0x12, 0x56, 0x8b, 0x44, 0x8d
};

static uint32_t Key05[] __attribute__((aligned(128)))= { 
	0xE0F67504,
	0xFAFB3850,
	0x90C3C7D2,
	0x3DCAB3ED,
	0x42124715,
	0x8A1EAE91,
	0x9ECD792F,
	0xBDEFBCD2
};

static const uint32_t S01[] = { 
	0xC3A7802A, 0x47E3A8FF 
};

static const uint32_t Text01[] = {
	0xCCCCCCCC, 0x33333333,
	0x33333333, 0xCCCCCCCC
};

#define LINE(X) \
0x##X##4##X##5##X##6##X##7, \
0x##X##0##X##1##X##2##X##3, \
0x##X##C##X##D##X##E##X##F, \
0x##X##8##X##9##X##A##X##B

static const uint32_t Text02[] = {
	LINE(0),
	LINE(1),
	LINE(2),
	LINE(3),
	LINE(4),
	LINE(5),
	LINE(6),
	LINE(7),
	LINE(8),
	LINE(9),
	LINE(A),
	LINE(B),
	LINE(C),
	LINE(D),
	LINE(E),
	LINE(F)
};

static const uint32_t CFB_Test05[] = {
	0x43224C3B, 0x4B64BD10, 
	0x5548EB99, 0x0FAA6CD9
};

static uint32_t IMIT_Test06[] = {
	0x46738F54
};

int main(int argc, char **argv)
{
	static char buf[1024] __attribute__((aligned(32))) = {0};
	
	static uint32_t tab[256 * 4] __attribute__((aligned(128)));
	gost_set_sbox(FapsiSubst, tab);
	
	gostiv_t iv;
	memcpy(&iv, S01, sizeof(iv));
	
	unsigned int n = 0;

	// Потестируем правильность алгоритмов.
	gost_encrypt_cfb((const char *)Text01, buf, sizeof(Text01), Key05, iv, &n, tab);
	assert(memcmp(buf, CFB_Test05, sizeof(CFB_Test05)) == 0);
	
	gost_imit_long((const char *)Text02, sizeof(Text02), Key05, tab, (uint32_t *)iv);
	assert(memcmp(iv, IMIT_Test06, sizeof(IMIT_Test06)) == 0);
		
	// Алгоритмы корректные - поехали.
	struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
	
	// меряем на гигабайте...
	for (int i = 0; i < 1024 * 1024; i++) {
		gost_encrypt_cfb(buf, buf, sizeof(buf), Key05, iv, &n, tab);
		gost_imit_long(buf, sizeof(buf), Key05, tab, (uint32_t *)&iv);
	}
	
	struct timespec stop;
	clock_gettime(CLOCK_REALTIME, &stop);
	
	double run_time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1000000000.0;
	double bps = 1024 * 1024 * 1024 / run_time;
	double bitps = bps * 8;
	
	printf("throughput: %.3f megabit/sec\n", bitps / 1000000);;
	return 0;
}
