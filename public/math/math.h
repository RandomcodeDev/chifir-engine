/// \file Miscellaneous math functions. All angles are in radians unless otherwise stated.
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

static constexpr f32 PI = 3.141592653589793f;
static constexpr f32 HALF_PI = PI / 2.0f;
static constexpr f32 TAU = PI * 2.0f;
// Can't just have a global constant named "E", but I don't really use namespaces
static constexpr f32 EULER_NUMBER = 2.718281828459045f;

FORCEINLINE f32 Deg2Rad(f32 theta)
{
    return theta * PI / 180.0f;
}

FORCEINLINE f32 Rad2Deg(f32 theta)
{
    return theta * 180.0f / PI;
}

// Inverse square root
FORCEINLINE f32 Rsqrt(f32 x);

// Square root
FORCEINLINE f32 Sqrt(f32 x);

// Sine
FORCEINLINE f32 Sin(f32 x);

// Cosine
FORCEINLINE f32 Cos(f32 x);

// Tangent
FORCEINLINE f32 Tan(f32 x);

// Inverse sine
FORCEINLINE f32 Asin(f32 x);

// Inverse cosine
FORCEINLINE f32 Acos(f32 x);

// Inverse tangent
FORCEINLINE f32 Atan(f32 x);

#ifdef CH_KNOWN_SIMD128
#ifdef CH_X86
#include "impl/math_x86.inl"
#endif
#else
#include "impl/math_generic.inl"
#endif
