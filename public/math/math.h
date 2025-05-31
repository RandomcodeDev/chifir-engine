/// \file Miscellaneous math functions
/// \copyright 2025 Randomcode Developers

#include "base/compiler.h"
#include "base/types.h"

FORCEINLINE f32 Rsqrt(f32 x)
{
    f32 Sqrt(f32 x);
#ifdef CH_KNOWN_SIMD128
    return 1.0f / Sqrt(x);
#else
	// quake 3 my beloved
	f32 x2 = x * 0.5;
	f32 y = x;
	u32 i = *(u32*)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(f32*)&i;
	y = y * (1.5 - (x2 * y * y));
	return y;
#endif
}

FORCEINLINE f32 Sqrt(f32 x)
{
#ifdef CH_KNOWN_SIMD128
#ifdef CH_X86
    const v128 v = _mm_load1_ps(&x);
    const v128 r = _mm_sqrt_ss(v);
    return _mm_cvtss_f32(r);
#endif
#else
    return 1.0f / Rsqrt(x);
#endif
}
