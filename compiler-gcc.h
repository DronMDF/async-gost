#ifndef __COMPILER_GCC_H__
#define __COMPILER_GCC_H__

#include <features.h>

#if __GNUC_PREREQ (4,8)
	/* gcc 4.8 has __builtin_cpu_init and others */
#else
# define __builtin_cpu_init(x)		(0)
# define __builtin_cpu_supports(x)	(0)
#endif

#endif /* __COMPILER_GCC_H__ */