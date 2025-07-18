/// \file x86 math implementation stuff
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "math/math.h"

FORCEINLINE f32 Math_ExtractLowerF32(v128 v)
{
#ifdef CH_KNOWN_SIMD256
    return _mm_cvtss_f32(v);
#else
	f32 ret;
    _mm_store_ss(&ret, v);
    return ret;
#endif
}

FORCEINLINE f32 Math_SumElems(v128 x)
{
	// https://stackoverflow.com/a/35270026
#ifdef CH_KNOWN_SIMD256
	v128 shuf = _mm_movehdup_ps(x);
#else
	v128 shuf = _mm_shuffle_ps(x, x, _MM_SHUFFLE(2, 3, 0, 1));
#endif
	v128 sums = _mm_add_ps(x, shuf);
	shuf = _mm_movehl_ps(shuf, sums);
	sums = _mm_add_ss(sums, shuf);
    return Math_ExtractLowerF32(sums);
}

FORCEINLINE f32 Rsqrt(f32 x)
{
    const v128 v = _mm_load1_ps(&x);
    const v128 r = _mm_rsqrt_ss(v);
    return Math_ExtractLowerF32(r);
}

FORCEINLINE f32 Sqrt(f32 x)
{
    const v128 v = _mm_load1_ps(&x);
    const v128 r = _mm_sqrt_ss(v);
    return Math_ExtractLowerF32(r);
}

// TODO: x86 doesn't really have anything better than x87 for these, so they're the generic versions

FORCEINLINE f32 Sin(f32 x)
{
    return Cos(HALF_PI - x);
}

FORCEINLINE f32 Cos(f32 x)
{
    // TODO: normalize once fmod is implemented
    x *= x;
    return 1 - x / 2 * (1 - x / 12 * (1 - x / 30 * (1 - x / 56 * (1 - x / 90))));
}

FORCEINLINE f32 Tan(f32 x)
{
    return Sin(x) / Cos(x);
}
FORCEINLINE f32 Asin(f32 x)
{
    return Atan(x / Sqrt(1 - x * x));
}

FORCEINLINE f32 Acos(f32 x)
{
    return HALF_PI - Asin(x);
}

FORCEINLINE f32 Atan(f32 x)
{
    f32 xp = x;
    const f32 t1 = xp;
    xp *= x * x; // x^3
    const f32 t2 = t1 - xp / 3.0f;
    xp *= x * x; // x^5
    const f32 t3 = t2 + xp / 5.0f;
    xp *= x * x; // x^7
    const f32 t4 = t3 - xp / 7.0f;
    xp *= x * x; // x^9
    const f32 t5 = t4 + xp / 9.0f;
    xp *= x * x; // x^11
    const f32 t6 = t5 - xp / 11.0f;
    xp *= x * x; // x^13
    const f32 t7 = t6 + xp / 13.0f;
    xp *= x * x; // x^15
    const f32 t8 = t7 - xp / 15.0f;
    return t8;
}
