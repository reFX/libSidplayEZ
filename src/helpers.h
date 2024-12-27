#pragma once

#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
	#define sidinline __forceinline
#else
	#define sidinline inline __attribute__((always_inline))
#endif
