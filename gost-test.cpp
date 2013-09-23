
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/systm.h>
#include <machine/cpufunc.h>
#include <contrib/ipcrypt/gost/gost.h>
#include <contrib/ipcrypt/gost/test.h>

const u_char FapsiSubst[] = {
	0xc4, 0xed, 0x83, 0xc9, 0x92, 0x98, 0xfe, 0x6b,
	0xff, 0xbe, 0x65, 0x5c, 0xe5, 0x2c, 0xb9, 0x20,
	0x89, 0x57, 0x16, 0xb3, 0x11, 0xf3, 0x98, 0x06,
	0x30, 0x79, 0xc0, 0x97, 0xa8, 0x1a, 0x5d, 0xd5,
	0x2e, 0x01, 0xda, 0x34, 0x73, 0xd5, 0x3b, 0xe8,
	0x4b, 0xc2, 0x77, 0x7e, 0xdc, 0x64, 0xac, 0xaf,
	0x6d, 0xa6, 0x02, 0xf1, 0x07, 0x4f, 0xe1, 0x4a,
	0xba, 0x30, 0x2f, 0x12, 0x56, 0x8b, 0x44, 0x8d
};

u_int32_t Key01[] = { 
	0x00000000, 
	0x00000000, 
	0x00000000, 
	0x00000000,
	0xFFFFFFFF, 
	0xFFFFFFFF, 
	0xFFFFFFFF, 
	0xFFFFFFFF 
};

u_int32_t Key02[] = { 
	0xFFFFFFFF, 
	0xFFFFFFFF, 
	0xFFFFFFFF, 
	0xFFFFFFFF, 
	0x00000000, 
	0x00000000, 
	0x00000000, 
	0x00000000
};

u_int32_t Key03[] = { 
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA
};

u_int32_t Key04[] = { 
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555
};

u_int32_t Key05[] = { 
	0xE0F67504,
	0xFAFB3850,
	0x90C3C7D2,
	0x3DCAB3ED,
	0x42124715,
	0x8A1EAE91,
	0x9ECD792F,
	0xBDEFBCD2
};

u_int32_t S01[] = { 
	0xC3A7802A, 
	0x47E3A8FF 
};

u_int32_t Text01[] = {
	0xCCCCCCCC, 0x33333333,
	0x33333333, 0xCCCCCCCC
};


u_int32_t Text02[] = {
	TEST_LINE(0),
	TEST_LINE(1),
	TEST_LINE(2),
	TEST_LINE(3),
	TEST_LINE(4),
	TEST_LINE(5),
	TEST_LINE(6),
	TEST_LINE(7),
	TEST_LINE(8),
	TEST_LINE(9),
	TEST_LINE(A),
	TEST_LINE(B),
	TEST_LINE(C),
	TEST_LINE(D),
	TEST_LINE(E),
	TEST_LINE(F)
};

u_int32_t Text03[] = {
	TEST_LINE(0),
	TEST_LINE(1),
	TEST_LINE(2),
	TEST_LINE(3),
	TEST_LINE(4),
	TEST_LINE(5),
	TEST_LINE(6),
	TEST_LINE(7),
	TEST_LINE(8),
	TEST_LINE(9),
	TEST_LINE(A),
	TEST_LINE(B),
	TEST_LINE(C),
	TEST_LINE(D),
	TEST_LINE(E),
	TEST_LINE(F),
	0xEFFA6329	
}; //use size 257!

/* Замена */


/*  Тест  1 */

/*  Используются:		Открытый текст То-1 */
/*  Заполнение КЗУ:		КЗУ-1 */

u_int32_t ECB_Test01[] = {
	0xF5FE5211, 0x17E8D02E,
	0x6390ED97, 0x3A962C89
};

/* //////////////////////////////////////////////////////////// */
/* // Тест 2 */
/* // */
/* // Используются:		Открытый текст То-1 */
/* // Заполнение КЗУ:		КЗУ-2 */
/* //////////////////////////////////////////////////////////// */
u_int32_t ECB_Test02[] = {
	0x2A78B7E0, 0x800A0268,
	0x462DA336, 0xEAB90129
};

/* //////////////////////////////////////////////////////////// */
/* // Тест 3 */
/* // */
/* // Используются:		Открытый текст То-1 */
/* // Заполнение КЗУ:		КЗУ-3 */
/* //////////////////////////////////////////////////////////// */
u_int32_t ECB_Test03[] = {
	0x8BB8CF97, 0x533CDA6B,
	0xBE407AB5, 0x5C055B4F
};

/* //////////////////////////////////////////////////////////// */
/* // Тест 4 */
/* // */
/* // Используются:		Открытый текст То-1 */
/* // Заполнение КЗУ:		КЗУ-4 */
/* //////////////////////////////////////////////////////////// */
u_int32_t ECB_Test04[] = {
	0x895A9742, 0x02DB134C,
	0xDAA70325, 0xB95DDC39
};
	
/* //////////////////////////////////////////////////////////// */
/* // Тест 5 */
/* // */
/* // Используются:		Открытый текст То-1 */
/* // Заполнение КЗУ:		КЗУ-5 */
/* //////////////////////////////////////////////////////////// */
u_int32_t ECB_Test05[] = {
	0x401EBED9, 0x56F5D77D,
	0x4E790503, 0x73FE0118
};

/* //////////////////////////////////////////////////////////// */
/* // Тест  6 */
/* // */
/* // Используются:		Открытый текст То-2 */
/* // Заполнение КЗУ:		КЗУ-5 */
/* //////////////////////////////////////////////////////////// */
u_int32_t ECB_Test06[] = {
	0x984C8C4B, 0xEA4AF215,
	0x0957C31E, 0xD12EBCB3,
	0x22F2D1E0, 0x18592D65,
	0x80FCDFF7, 0x685CDE4B,
	0x53755346, 0xEC0D46A7,
	0xD31B1F05, 0xB71A630A,
	0xE043C478, 0x0EA43E5D,
	0xA9237E2D, 0xBC02C91B,
	0xCB840C21, 0xC8070A0D,
	0xB5FBD07B, 0x5C04141A,
	0x719753A2, 0x8FC25C2E,
	0x526F3F39, 0x4E2630F2,
	0x01D1E08C, 0xD3DC6D75,
	0xCA1E7903, 0x120EC1D5,
	0xE2780A53, 0xEA1CB10A,
	0xB955F83A, 0xBA0BE17C,
	0xEB96C8A0, 0x60D35A50,
	0x980FA343, 0x6D50D9DB,
	0x01AF9163, 0x5A75E940,
	0x191F5C46, 0x9B890B4A,
	0xF5F8F6C4, 0xFA3F872F,
	0x25F8D426, 0x82981FBA,
	0x2DAF26FC, 0x58C4F9C0,
	0x8009FA49, 0x34A46202,
	0x6B5ACB2D, 0x085D61AB,
	0x08E026D4, 0x022ED613,
	0xD0E8372A, 0xC7F136CF,
	0x219B3FC0, 0x2D29BD60,
	0x4E48012E, 0x16208FF8,
	0xDC82BF8A, 0x18A37A32,
	0x5950D169, 0x6CF29131,
	0x58CA5F5A, 0xB22DB29A
};

/* // Гаммирование с обратной связью */

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  1 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-1 */
/* //	Синхропосылка     S1 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t CFB_Test01[] = {
	0x28A5FC57, 0xD6F0A95A, 
	0x84626C1A, 0xC4DF8CC1
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  2 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-2 */
/* //	Синхропосылка     S1 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t CFB_Test02[] = {
	0x3B6A55EC, 0x5508C3A5, 
	0x2172F124, 0x8EA70D76
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  3 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-3 */
/* //	Синхропосылка     S1 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t CFB_Test03[] = {
	0x6815713D, 0x0F11C424, 
	0xB3A22BE2, 0xC2003BE0
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест 4 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-4 */
/* //	Синхропосылка     S1 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t CFB_Test04[] = {
	0x655F02F8, 0x25A67889,
	0x7C21350B, 0x79FBE22F
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест	5					 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-5 */
/* //	Синхропосылка     S1 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t CFB_Test05[] = {
	0x43224C3B, 0x4B64BD10, 
	0x5548EB99, 0x0FAA6CD9
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест	6					 */
/* // */
/* //	Используются:  Открытый текст    То-2 */
/* //	Заполнение КЭУ    КЗУ-5 */
/* //	Синхропосылка     S2 */
/* ////////////////////////////////////////////////////////////////////// */
/* //static unsigned cha Test06[] = { */
/* //}; */

/* // Имитовставка */

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  1 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-1 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t IMIT_Test01[] = {
	0xFD4BBE83
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  2 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-2 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t IMIT_Test02[] = {
	0xA8B979F7
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  З */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-3 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t IMIT_Test03[] = {
	0xD461FE01
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  4 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-4 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t IMIT_Test04[] = {
	0x12394623
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  5 */
/* // */
/* //	Используются:  Открытый текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-5 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t IMIT_Test05[] = {
	0x5AC9272C
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  6 */
/* // */
/* //	Используются:  Открытый текст    То-2 */
/* //	Заполнение КЭУ    КЗУ-5 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t IMIT_Test06[] = {
	0x46738F54
};

/* ////////////////////////////////////////////////////////////////////// */
/* //	Тест  1 */
/* // */
/* //	Используются:  Шифрованный в режиме CFB текст    То-1 */
/* //	Заполнение КЭУ    КЗУ-1 */
/* ////////////////////////////////////////////////////////////////////// */
u_int32_t IMIT_CFB_Text01[] = {
	0xb0f57479
};

u_int32_t C_IMIT_CFB_Text02[] = {
	0x5ec25a13
};

u_int32_t S_IMIT_CFB_Text02[] = {
	0xbb5c6cbe
};

u_int32_t IMIT_CFB_Text02_124[] = {
	0xb4bdbcce
};

u_int32_t C_IMIT_CFB_Text02_124[] = {
	0x79abe735
};

u_int32_t S_IMIT_CFB_Text02_124[] = {
	0x57839f24
};

u_int32_t S_IMIT_CFB_Text03_257[] = {
	0x1a7423e3
};

u_int32_t C_IMIT_CFB_Text03_257[] = {
	0x684a9ebd
};

u_int32_t CFB_Text02[] = {
0xe06c369c,
0xe5c2986a,
0xf1d71c10,
0x1a56d16f,
0x6ddda7d2,
0x626b7973,
0x3b446656,
0x2d817a22,
0x9f7c8acd,
0xc6a5ceb4,
0x24e054f2,
0x87bab94c,
0xc6d2991b,
0xba76f7ca,
0xdc755ea7,
0x219c65fb,
0x938e957f,
0x3d4bced1,
0xe06974d4,
0x0f5ea8da,
0x2686a64d,
0x36f58951,
0xcb31ddf9,
0x53a56489,
0xdf299200,
0xf748266a,
0x00e9bb6a,
0x48129b98,
0x3f16959f,
0xa5f78419,
0x4e295620,
0xbd76aae5,
0x7b39e290,
0xc7f7ae05,
0xef252159,
0x7aeb7668,
0x7135b4fa,
0x5d32c343,
0x8989da1a,
0xb0e211d2,
0x406fc7ee,
0xed75b570,
0xba8a0d2b,
0x21ffe348,
0x07e02c52,
0x047bf130,
0x1fe8ddf6,
0x895276ba,
0xe107a944,
0xd4fdea78,
0xcb22b6a8,
0x165d4c69,
0x3a9f47a5,
0x04e0a832,
0xff996958,
0x7b2fb161,
0x530db53e,
0x25b72894,
0x6054c44c,
0x3f8c1850,
0x7fef40a9,
0xe77809d6,
0xf973ff13,
0x405f53f0
};

u_int32_t S_CFB_Text02[] = {
0xd62ad25a,
0x3a9eb740,
0x8a866c04,
0xb587b52f,
0x02302196,
0xa4aa5b22,
0x05012aed,
0xaa24a925,
0x6b5ecd1c,
0xf505c5b8,
0x51895512,
0x187b733f,
0xf9dc5c6c,
0x6a693140,
0xd8540ea4,
0xddfaa052,
0x6e161f76,
0x76b40304,
0x321879c5,
0xc1841c0c,
0x7304e5a8,
0xb1378f20,
0x426624dd,
0x47315fb3,
0x00629b39,
0x7a23aa62,
0x4e2f3c92,
0xddde4a99,
0x352de5a3,
0x714f41fd,
0x74a5ed92,
0xda6fed1c,
0xcf7bf48f,
0xdad04c96,
0x62801c10,
0xd5715f3c,
0xbc1f2ec4,
0x87e71f6c,
0x20c5eac8,
0x9025fbf6,
0x32e94866,
0x87394b17,
0xda93f31b,
0x8bfc94a9,
0x0f91af1a,
0xe5cc5c3a,
0x438c5327,
0x81319059,
0x7f0fead9,
0x6d91311d,
0x25d64e25,
0xa276515d,
0xf8a23ae7,
0xe9047993,
0xa6eb7725,
0x60971ec2,
0x3410a0fc,
0x3b2ddc34,
0x409e91d4,
0x7ce46150,
0x79d0e81f,
0xd5dc5fea,
0xfd1765a7,
0xd1e6a5c3
};

u_int32_t S_CFB_Text02_124[] = {
0xd62ad25a,
0x3a9eb740,
0x8a866c04,
0xb587b52f,
0x02302196,
0xa4aa5b22,
0x05012aed,
0xaa24a925,
0x6b5ecd1c,
0xf505c5b8,
0x51895512,
0x187b733f,
0xf9dc5c6c,
0x6a693140,
0xd8540ea4,
0xddfaa052,
0x6e161f76,
0x76b40304,
0x321879c5,
0xc1841c0c,
0x7304e5a8,
0xb1378f20,
0x426624dd,
0x47315fb3,
0x00629b39,
0x7a23aa62,
0x4e2f3c92,
0xddde4a99,
0x352de5a3,
0x714f41fd,
0x74a5ed92
};

char S_CFB_Text03_257[] = {
0x5a,0xd2,0x2a,0xd6,
0x40,0xb7,0x9e,0x3a,
0x04,0x6c,0x86,0x8a,
0x2f,0xb5,0x87,0xb5,
0x96,0x21,0x30,0x02,
0x22,0x5b,0xaa,0xa4,
0xed,0x2a,0x01,0x05,
0x25,0xa9,0x24,0xaa,
0x1c,0xcd,0x5e,0x6b,
0xb8,0xc5,0x05,0xf5,
0x12,0x55,0x89,0x51,
0x3f,0x73,0x7b,0x18,
0x6c,0x5c,0xdc,0xf9,
0x40,0x31,0x69,0x6a,
0xa4,0x0e,0x54,0xd8,
0x52,0xa0,0xfa,0xdd,
0x76,0x1f,0x16,0x6e,
0x04,0x03,0xb4,0x76,
0xc5,0x79,0x18,0x32,
0x0c,0x1c,0x84,0xc1,
0xa8,0xe5,0x04,0x73,
0x20,0x8f,0x37,0xb1,
0xdd,0x24,0x66,0x42,
0xb3,0x5f,0x31,0x47,
0x39,0x9b,0x62,0x00,
0x62,0xaa,0x23,0x7a,
0x92,0x3c,0x2f,0x4e,
0x99,0x4a,0xde,0xdd,
0xa3,0xe5,0x2d,0x35,
0xfd,0x41,0x4f,0x71,
0x92,0xed,0xa5,0x74,
0x1c,0xed,0x6f,0xda,
0x8f,0xf4,0x7b,0xcf,
0x96,0x4c,0xd0,0xda,
0x10,0x1c,0x80,0x62,
0x3c,0x5f,0x71,0xd5,
0xc4,0x2e,0x1f,0xbc,
0x6c,0x1f,0xe7,0x87,
0xc8,0xea,0xc5,0x20,
0xf6,0xfb,0x25,0x90,
0x66,0x48,0xe9,0x32,
0x17,0x4b,0x39,0x87,
0x1b,0xf3,0x93,0xda,
0xa9,0x94,0xfc,0x8b,
0x1a,0xaf,0x91,0x0f,
0x3a,0x5c,0xcc,0xe5,
0x27,0x53,0x8c,0x43,
0x59,0x90,0x31,0x81,
0xd9,0xea,0x0f,0x7f,
0x1d,0x31,0x91,0x6d,
0x25,0x4e,0xd6,0x25,
0x5d,0x51,0x76,0xa2,
0xe7,0x3a,0xa2,0xf8,
0x93,0x79,0x04,0xe9,
0x25,0x77,0xeb,0xa6,
0xc2,0x1e,0x97,0x60,
0xfc,0xa0,0x10,0x34,
0x34,0xdc,0x2d,0x3b,
0xd4,0x91,0x9e,0x40,
0x50,0x61,0xe4,0x7c,
0x1f,0xe8,0xd0,0x79,
0xea,0x5f,0xdc,0xd5,
0xa7,0x65,0x17,0xfd,
0xc3,0xa5,0xe6,0xd1,
0x2c
};

static u_int test_tab[256 * 4];

static int
ecbtest(u_int *plain, u_int *key, u_int *cipher) {
	u_char buf[8];

	bcopy(plain, buf, 8);
	gost_encrypt((int *)buf, (int *)key, test_tab);
	
	return (bcmp(cipher, buf, 8))?1:0;
}

static int
de_ecbtest(u_int *cipher, u_int *key, u_int *plain) {
	u_char buf[8];

	bcopy(cipher, buf, 8);
	gost_decrypt((int *)buf, (int *)key, test_tab);

	return (bcmp(plain, buf, 8))?1:0;
}

static int
cfbtest(u_int *plain, u_int *key, u_int *init, u_int *cipher) {
	u_char buf[8];
	u_char syn[8];
	u_int n;

	n = 0;
	bcopy(init, syn, 8);
	gost_encrypt_cfb((char *)plain, buf, 8, key, syn, &n, test_tab);
	
	return (bcmp(cipher, buf, 8))?1:0;
}

static int
de_cfbtest(u_int *cipher, u_int *key, u_int *init, u_int *plain) {
	u_char buf[8];
	u_char syn[8];
	u_int n;

	n = 0;
	bcopy(init, syn, 8);
	gost_encrypt_cfb((char *)cipher, buf, 8, key, syn, &n, test_tab);
	
	return (bcmp(plain, buf, 8))?1:0;
}

static int
imittest(u_int *plain, u_int *key, u_int *imit, size_t len) {
	u_char buf[8];
	u_int i;
	u_char *data = (u_char *)plain;

	bzero(buf, 8);
	
	while (len > 0) {
		for (i = 0; i < 8 ; i++)
			buf[i] ^= *(data++);
		gost_imit((int *)buf, (int *)key, test_tab);
		len -= 8;
	}
	

	return (bcmp(imit, buf, 4))?1:0;
}
static int
test_splited_data_m(u_int op, u_int *in, u_int *out, u_int size, u_int *key, u_int *iv) {
	 
	struct mbuf* m;
	int ret = 0;
	u_int imit;

	MGETHDR(m, M_WAIT, MT_DATA);	
	
	if (m == NULL) {
                return 1;
        }
   
        m->m_len = m->m_pkthdr.len = 0;

	m_copyback(m, 0, size, (caddr_t)in);

	switch(op) {
		case 0:
			gost_encrypt_sd_m(m, 0, key, (void*) iv, (const void*)test_tab);
			break;
		case 1:
			gost_decrypt_sd_m(m, 0, key, (void*) iv, (const void*)test_tab);
			break;
		case 2:
			imit = gost_imit_sm_m(m, 0, key, (const void*)test_tab);
			break;
		case 3:
			imit = gost_imit_m(m, 0, 0, key, (const void*)test_tab);
			break;
		default:
			panic("undefined operation type");
	}
	
	if (op < 2) {
		struct mbuf* cm = m;
		while (cm) {
			if( (ret = bcmp(mtod(cm,char *), out, cm->m_len)?1:0)  ) {
				break;
			}
			out += cm->m_len/sizeof(u_int);
			cm = cm->m_next;
		}
	} else {
		ret = bcmp(&imit, out, sizeof(imit)) ? 1:0;
	}

	m_freem(m);
	return ret;
}


const char *res_str[2] = { ".", "X" };

int
gost_selftest(void) {
	int res;
	int n;

	res = 0;
	
	gost_set_sbox(FapsiSubst, test_tab);

	printf("gost: selftest\n");
	printf("\tECB: ");
	n = ecbtest(Text01, Key01, ECB_Test01);
	printf("%s", res_str[n]);
	res += n;
	n = ecbtest(Text01, Key02, ECB_Test02);
	printf("%s", res_str[n]);
	res += n;
	n = ecbtest(Text01, Key03, ECB_Test03);
	printf("%s", res_str[n]);
	res += n;
	n = ecbtest(Text01, Key04, ECB_Test04);
	printf("%s", res_str[n]);
	res += n;
	n = ecbtest(Text01, Key05, ECB_Test05);
	printf("%s", res_str[n]);
	res += n;
	n = ecbtest(Text02, Key05, ECB_Test06);
	printf("%s", res_str[n]);
	res += n;
	n = de_ecbtest(ECB_Test01, Key01, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_ecbtest(ECB_Test02, Key02, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_ecbtest(ECB_Test03, Key03, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_ecbtest(ECB_Test04, Key04, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_ecbtest(ECB_Test05, Key05, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_ecbtest(ECB_Test06, Key05, Text02);
	printf("%s", res_str[n]);
	res += n;

	printf("\n\tCFB: ");
	n = cfbtest(Text01, Key01, S01, CFB_Test01);
	printf("%s", res_str[n]);
	res += n;
	n = cfbtest(Text01, Key02, S01, CFB_Test02);
	printf("%s", res_str[n]);
	res += n;
	n = cfbtest(Text01, Key03, S01, CFB_Test03);
	printf("%s", res_str[n]);
	res += n;
	n = cfbtest(Text01, Key04, S01, CFB_Test04);
	printf("%s", res_str[n]);
	res += n;
	n = cfbtest(Text01, Key05, S01, CFB_Test05);
	printf("%s", res_str[n]);
	res += n;
	n = de_cfbtest(CFB_Test01, Key01, S01, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_cfbtest(CFB_Test02, Key02, S01, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_cfbtest(CFB_Test03, Key03, S01, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_cfbtest(CFB_Test04, Key04, S01, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = de_cfbtest(CFB_Test05, Key05, S01, Text01);
	printf("%s", res_str[n]);
	res += n;
	n = test_splited_data_m(0, Text02, S_CFB_Text02, sizeof(Text02), Key01, S01);
	printf("%s", res_str[n]);
	res += n;
	n = test_splited_data_m(1, S_CFB_Text02, Text02, sizeof(Text02), Key01, S01);
	printf("%s", res_str[n]);
	res += n;
	n = test_splited_data_m(0, Text03, (void*)S_CFB_Text03_257, 257, Key01, S01);
	printf("%s", res_str[n]);
	res += n;
	n = test_splited_data_m(1, (void*)S_CFB_Text03_257, Text03, 257, Key01, S01);
	printf("%s", res_str[n]);
	res += n;

	printf("\n\tIMIT: ");
	n = imittest(Text01, Key01, IMIT_Test01, 16);
	printf("%s", res_str[n]);
	res += n;
	n = imittest(Text01, Key02, IMIT_Test02, 16);
	printf("%s", res_str[n]);
	res += n;
	n = imittest(Text01, Key03, IMIT_Test03, 16);
	printf("%s", res_str[n]);
	res += n;
	n = imittest(Text01, Key04, IMIT_Test04, 16);
	printf("%s", res_str[n]);
	res += n;
	n = imittest(Text01, Key05, IMIT_Test05, 16);
	printf("%s", res_str[n]);
	res += n;
	n = imittest(Text02, Key05, IMIT_Test06, sizeof(Text02));
	printf("%s", res_str[n]);
	res += n;
	n = test_splited_data_m(2, S_CFB_Text02, S_IMIT_CFB_Text02, sizeof(S_CFB_Text02), Key01, S01);
	printf("%s", res_str[n]);
	res += n;
	n = test_splited_data_m(3, (void*)S_CFB_Text03_257, C_IMIT_CFB_Text03_257, 257, Key01, S01);
	printf("%s", res_str[n]);
	res += n;
	n = test_splited_data_m(2, (void*)S_CFB_Text03_257, S_IMIT_CFB_Text03_257, 257, Key01, S01);
	printf("%s", res_str[n]);
	res += n;
	
	printf("\n\t%d errors\n", res);
	
	return res;
}
