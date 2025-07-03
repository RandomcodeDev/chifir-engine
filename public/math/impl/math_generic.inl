/// \file Generic implementations of math functions
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "math/math.h"

FORCEINLINE f32 Rsqrt(f32 x)
{
    // quake 3 my beloved. this is faster than x87 fsqrt.
    f32 x2 = x * 0.5;
    f32 y = x;
    u32 i = *(u32*)&y;
    i = 0x5F375A86 - (i >> 1);
    y = *(f32*)&i;
    y = y * (1.5 - (x2 * y * y));
    return y;
}

FORCEINLINE f32 Sqrt(f32 x)
{
    return 1.0f / Rsqrt(x);
}

// TODO: determine if there's a better way to implement these, even if they're fallbacks

FORCEINLINE f32 Sin(f32 x)
{
    return Cos(HALF_PI - x);
}

// https://stackoverflow.com/questions/77723846/implementing-a-cosine-function-in-c
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

// TODO: rewrite these once i'm better with taylor series

FORCEINLINE f32 Asin(f32 x)
{
    return Atan(x / Sqrt(1 - x * x));
}

FORCEINLINE f32 Acos(f32 x)
{
    return HALF_PI - Asin(x);
}

// accurate up to +-~0.97
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
