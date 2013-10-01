
#include <features.h>

bool cpu_support_ssse3()
{
#ifdef __GNUC__
#if __GNUC_PREREQ(4,8)
	__builtin_cpu_init();
	return __builtin_cpu_supports("ssse3");
#endif
#endif
	return true;
}

bool cpu_support_avx()
{
	return true;
}

bool cpu_support_avx2()
{
	return true;
}
