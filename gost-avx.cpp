
#define _POSIX_C_SOURCE 200112L

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned v4si __attribute__ ((vector_size (16)));
typedef unsigned v2si __attribute__ ((vector_size (8)));
typedef unsigned char v16qi __attribute__ ((vector_size (16)));

typedef uint8_t gostiv_t[8];
typedef uint32_t gostkey_t[8];

static const v4si lo_mask = { 0x000f000f, 0x000f000f, 0x000f000f, 0x000f000f };
static const v4si hi_mask = { 0x0f000f00, 0x0f000f00, 0x0f000f00, 0x0f000f00 };

static const v16qi b0_mask = { 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 
				0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00 };
static const v16qi b1_mask = { 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 
				0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00 };
static const v16qi b2_mask = { 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 
				0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0 };
static const v16qi b3_mask = { 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 
				0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f };
	
static
void gost_encrypt_block (uint32_t *block, const v4si *key, const v16qi *tab)
{
	v4si a = { block[0], block[2], block[4], block[6] };
	v4si b = { block[1], block[3], block[5], block[7] };

	for (int i = 0; i < 32; i++) {
		const v4si lo = key[i] + a;
		const v4si hi = __builtin_ia32_psrldi128(lo, 4);
		const v4si loc = (lo & lo_mask) | (hi & hi_mask);
		const v4si hic = (lo & hi_mask) | (hi & lo_mask);
		const v16qi b0 = __builtin_ia32_pshufb128(tab[0], loc) & b0_mask; // 0x????1??0
		const v16qi b1 = __builtin_ia32_pshufb128(tab[1], hic) & b1_mask; // 0x?????10?
		const v16qi b2 = __builtin_ia32_pshufb128(tab[2], loc) & b2_mask; // 0x3??2????
		const v16qi b3 = __builtin_ia32_pshufb128(tab[3], hic) & b3_mask; // 0x?32?????
		const v4si i1 = b0 | b1 | b2 | b3;
		const v4si i2 = b ^ (__builtin_ia32_pslldi128(i1, 11) | 
				__builtin_ia32_psrldi128(i1, 21));
		b = a;
		a = i2;
	}

	block[0] = ((uint32_t *)&b)[0];
	block[1] = ((uint32_t *)&a)[0];
	block[2] = ((uint32_t *)&b)[1];
	block[3] = ((uint32_t *)&a)[1];
	block[4] = ((uint32_t *)&b)[2];
	block[5] = ((uint32_t *)&a)[2];
	block[6] = ((uint32_t *)&b)[3];
	block[7] = ((uint32_t *)&a)[3];
}

// static
// void gost_imit_block (uint32_t *block, const v4si *key, const v16qi *sbox)
// {
// 	// TODO: Multithread
// 	v4si a = { block[1], 0, 0, 0 };
// 	v4si b = { block[0], 0, 0, 0 };
// 
// 	for (int i = 0; i < 16; i += 2) {
// 		a = gost_step(a, b, key[i], sbox);
// 		b = gost_step(b, a, key[i + 1], sbox);
// 	}
// 
// 	block[1] = ((uint32_t *)&a)[0];
// 	block[0] = ((uint32_t *)&b)[0];
// }
// 
// void gost_encrypt_cfb(const char *in, char *out, size_t size, const v4si *key, gostiv_t iv, uint32_t *n, const v16qi *tab) 
// {
// 	if (size < 16 - *n) {
// 		/* нет смысла извращаться */
// 		while (size--) {
// 			if (!(*n)) {
// 				gost_encrypt_block((uint32_t *)iv, key, tab);
// 			}
// 
// 			iv[*n] = *(out++) = iv[*n] ^ *(in++);
// 			(*n)++;
// 			(*n) &= 7;
// 		}
// 	} else {
// 		/* есть хотя бы один полный блок */
// 		if (*n) {
// 			size -= 8 - *n;
// 			while (*n != 8) {
// 				iv[*n] = *(out++) = iv[*n] ^ *(in++);
// 				(*n)++;
// 			}
// 			*n = 0;
// 		}
// 		while (size & (~7)) {
// 			gost_encrypt_block((uint32_t *)iv, key, tab);
// 			((int *)iv)[0] = ((int *)out)[0] = ((int *)iv)[0] ^ ((int *)in)[0];
// 			((int *)iv)[1] = ((int *)out)[1] = ((int *)iv)[1] ^ ((int *)in)[1];
// 			size -= 8;
// 			in += 8;
// 			out += 8;
// 		}
// 		if (size) {
// 			gost_encrypt_block((uint32_t *)iv, key, tab);
// 			while (size--) {
// 				iv[*n] = *(out++) = iv[*n] ^ *(in++);
// 				(*n)++;
// 			}
// 		}
// 	}
// }
// 
// void gost_imit_long(const char *buf, size_t size, const uint32_t *key, const v16qi *tab, uint32_t rv[2]) 
// {
// 	v4si vkey[32];
// 	for (int i = 0; i < 8; i++) {
// 		const v4si vk = { key[i], key[i], key[i], key[i] };
// 		vkey[i] = vkey[8 + i] = vkey[16 + i] = vkey[31 - i] = vk;
// 	}
// 	
// 	int n = 16;
// 	rv[0] = rv[1] = 0;
// 
// 	char *result = (char *)rv;
// 	for (size_t i = 0; i < size; i++) {
// 		if (n == 8) {
// 			gost_imit_block((uint32_t *)result, vkey, tab);
// 		}
// 		n &= 7;
// 		result[n++] ^= buf[i];
// 	}
// 	gost_imit_block((uint32_t *)result, vkey, tab);
// }

void gost_encrypt_ecb(const char *in, char *out, size_t size, const uint32_t *key, const v16qi *tab)
{
	v4si vkey[32];
	for (int i = 0; i < 8; i++) {
		const v4si vk = { key[i], key[i], key[i], key[i] };
		vkey[i] = vkey[8 + i] = vkey[16 + i] = vkey[31 - i] = vk;
	}
	
	assert(size % 8 * 4 == 0);
	
	if (out != in) {
		memcpy (out, in, size);
	}
	
	while (size > 0) {
		gost_encrypt_block((uint32_t *)out, vkey, tab);
		out += 8 * 4;
		if (size > 8 * 4) {
			size -= 8 * 4;
		} else {
			size = 0;
		}
	}
}

v16qi expand_tab(const uint8_t *sbox, int li, int hi)
{
	const v16qi lt = { 
		sbox[0 * 4 + li] & 0x0f, sbox[1 * 4 + li] & 0x0f, sbox[2 * 4 + li] & 0x0f, sbox[3 * 4 + li] & 0x0f,
		sbox[4 * 4 + li] & 0x0f, sbox[5 * 4 + li] & 0x0f, sbox[6 * 4 + li] & 0x0f, sbox[7 * 4 + li] & 0x0f,
		sbox[8 * 4 + li] & 0x0f, sbox[9 * 4 + li] & 0x0f, sbox[10 * 4 + li] & 0x0f, sbox[11 * 4 + li] & 0x0f,
		sbox[12 * 4 + li] & 0x0f, sbox[13 * 4 + li] & 0x0f, sbox[14 * 4 + li] & 0x0f, sbox[15 * 4 + li] & 0x0f };
	const v16qi ht = { 
		sbox[0 * 4 + hi] & 0xf0, sbox[1 * 4 + hi] & 0xf0, sbox[2 * 4 + hi] & 0xf0, sbox[3 * 4 + hi] & 0xf0,
		sbox[4 * 4 + hi] & 0xf0, sbox[5 * 4 + hi] & 0xf0, sbox[6 * 4 + hi] & 0xf0, sbox[7 * 4 + hi] & 0xf0,
		sbox[8 * 4 + hi] & 0xf0, sbox[9 * 4 + hi] & 0xf0, sbox[10 * 4 + hi] & 0xf0, sbox[11 * 4 + hi] & 0xf0,
		sbox[12 * 4 + hi] & 0xf0, sbox[13 * 4 + hi] & 0xf0, sbox[14 * 4 + hi] & 0xf0, sbox[15 * 4 + hi] & 0xf0 };
	return lt | ht; 
}

void gost_set_sbox(const uint8_t *sbox, v16qi *tab) 
{
	tab[0] = expand_tab(sbox, 0, 1);
	tab[1] = expand_tab(sbox, 1, 0);
	tab[2] = expand_tab(sbox, 2, 3);
	tab[3] = expand_tab(sbox, 3, 2);
}

static const uint8_t FapsiSubst[] = {
	0xc4, 0xed, 0x83, 0xc9, 0x92, 0x98, 0xfe, 0x6b,
	0xff, 0xbe, 0x65, 0x5c, 0xe5, 0x2c, 0xb9, 0x20,
	0x89, 0x57, 0x16, 0xb3, 0x11, 0xf3, 0x98, 0x06,
	0x30, 0x79, 0xc0, 0x97, 0xa8, 0x1a, 0x5d, 0xd5,
	0x2e, 0x01, 0xda, 0x34, 0x73, 0xd5, 0x3b, 0xe8,
	0x4b, 0xc2, 0x77, 0x7e, 0xdc, 0x64, 0xac, 0xaf,
	0x6d, 0xa6, 0x02, 0xf1, 0x07, 0x4f, 0xe1, 0x4a,
	0xba, 0x30, 0x2f, 0x12, 0x56, 0x8b, 0x44, 0x8d
};

static uint32_t Key01[] __attribute__((aligned(128))) = { 
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 

	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 

	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 

	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0x00000000, 0x00000000, 0x00000000, 0x00000000
};

static uint32_t Key02[] __attribute__((aligned(128))) = { 
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF 
};

static uint32_t Key03[] __attribute__((aligned(128))) = { 
	0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA,
	0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA,

	0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA,
	0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA,

	0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA,
	0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA,
	
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555
};

static uint32_t Key04[] __attribute__((aligned(128))) = { 
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,

	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,

	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,

	0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA,
	0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA
};

static uint32_t Key05[] __attribute__((aligned(128))) = { 
	0xE0F67504, 0xFAFB3850, 0x90C3C7D2, 0x3DCAB3ED,
	0x42124715, 0x8A1EAE91, 0x9ECD792F, 0xBDEFBCD2,

	// CFB optimizations
	0xE0F67504, 0xFAFB3850, 0x90C3C7D2, 0x3DCAB3ED,
	0x42124715, 0x8A1EAE91, 0x9ECD792F, 0xBDEFBCD2,

	0xE0F67504, 0xFAFB3850, 0x90C3C7D2, 0x3DCAB3ED,
	0x42124715, 0x8A1EAE91, 0x9ECD792F, 0xBDEFBCD2,

	0xBDEFBCD2, 0x9ECD792F, 0x8A1EAE91, 0x42124715,
	0x3DCAB3ED, 0x90C3C7D2, 0xFAFB3850, 0xE0F67504,
};

static const uint32_t S01[] = { 
	0xC3A7802A, 0x47E3A8FF 
};

static const uint32_t Text01[] = {
	0xCCCCCCCC, 0x33333333, 0x33333333, 0xCCCCCCCC
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

static const uint32_t ECB_Test01[] = {
	0xF5FE5211, 0x17E8D02E, 0x6390ED97, 0x3A962C89
};

static const uint32_t ECB_Test02[] = { 
	0x2A78B7E0, 0x800A0268, 0x462DA336, 0xEAB90129
};

static const uint32_t ECB_Test03[] = {
	0x8BB8CF97, 0x533CDA6B, 0xBE407AB5, 0x5C055B4F
};

static const uint32_t ECB_Test04[] = { 
	0x895A9742, 0x02DB134C, 0xDAA70325, 0xB95DDC39
};
	
static const uint32_t ECB_Test05[] = {
	0x401EBED9, 0x56F5D77D, 0x4E790503, 0x73FE0118
};

static const uint32_t ECB_Test06[] = {
	0x984C8C4B, 0xEA4AF215, 0x0957C31E, 0xD12EBCB3,
	0x22F2D1E0, 0x18592D65, 0x80FCDFF7, 0x685CDE4B,
	0x53755346, 0xEC0D46A7, 0xD31B1F05, 0xB71A630A, 
	0xE043C478, 0x0EA43E5D, 0xA9237E2D, 0xBC02C91B,
	0xCB840C21, 0xC8070A0D, 0xB5FBD07B, 0x5C04141A,
	0x719753A2, 0x8FC25C2E, 0x526F3F39, 0x4E2630F2,
	0x01D1E08C, 0xD3DC6D75, 0xCA1E7903, 0x120EC1D5,
	0xE2780A53, 0xEA1CB10A, 0xB955F83A, 0xBA0BE17C,
	0xEB96C8A0, 0x60D35A50, 0x980FA343, 0x6D50D9DB,
	0x01AF9163, 0x5A75E940, 0x191F5C46, 0x9B890B4A,
	0xF5F8F6C4, 0xFA3F872F, 0x25F8D426, 0x82981FBA,
	0x2DAF26FC, 0x58C4F9C0, 0x8009FA49, 0x34A46202,
	0x6B5ACB2D, 0x085D61AB, 0x08E026D4, 0x022ED613,
	0xD0E8372A, 0xC7F136CF, 0x219B3FC0, 0x2D29BD60,
	0x4E48012E, 0x16208FF8, 0xDC82BF8A, 0x18A37A32,
	0x5950D169, 0x6CF29131, 0x58CA5F5A, 0xB22DB29A
};

static const uint32_t CFB_Test05[] = {
	0x43224C3B, 0x4B64BD10, 
	0x5548EB99, 0x0FAA6CD9
};

// static uint32_t IMIT_Test06[] = {
// 	0x46738F54
// };

void ecbtest(const char *text, size_t size, const uint32_t *key, const v16qi *sbox, const uint32_t *expected) 
{
	char buf[size + 8 * 4];
	gost_encrypt_ecb(text, buf, size, key, sbox);
	assert(memcmp(buf, expected, size) == 0);
}

int main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	enum {
		block_size = 1600,
		test_size = 1024 * 1024 * 1024
	};
	
	static char buf[1024] __attribute__((aligned(32))) = {0};
	
	static v16qi tab[4];
	gost_set_sbox(FapsiSubst, tab);
	
	// Потестируем правильность алгоритмов.
// 	uint32_t rv[2];
// 	gost_imit_long((const char *)Text02, sizeof(Text02), Key05, tab, rv);
// 	assert(memcmp(rv, IMIT_Test06, sizeof(IMIT_Test06)) == 0);
	
	ecbtest((const char *)Text01, sizeof(Text01), Key01, tab, ECB_Test01);
	ecbtest((const char *)Text01, sizeof(Text01), Key02, tab, ECB_Test02);
	ecbtest((const char *)Text01, sizeof(Text01), Key03, tab, ECB_Test03);
	ecbtest((const char *)Text01, sizeof(Text01), Key04, tab, ECB_Test04);
	ecbtest((const char *)Text01, sizeof(Text01), Key05, tab, ECB_Test05);
	ecbtest((const char *)Text02, sizeof(Text02), Key05, tab, ECB_Test06);
	
// 	gostiv_t iv;
// 	memcpy(&iv, S01, sizeof(iv));
// 	unsigned int n = 0;
// 
// 	gost_encrypt_cfb((const char *)Text01, buf, sizeof(Text01), Key05, iv, &n, tab);
// 	assert(memcmp(buf, CFB_Test05, sizeof(CFB_Test05)) == 0);
	
	// Алгоритмы корректные - поехали.
	struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
	
	// меряем на гигабайте...
	for (int i = 0; i < test_size / block_size; i++) {
		gost_encrypt_ecb(buf, buf, sizeof(buf), Key05, tab);
	}
	
	struct timespec stop;
	clock_gettime(CLOCK_REALTIME, &stop);
	
	double run_time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1000000000.0;
	double bps = 1024 * 1024 * 1024 / run_time;
	double bitps = bps * 8;
	
	printf("throughput: %.3f megabit/sec\n", bitps / 1000000);;
	return 0;
}
