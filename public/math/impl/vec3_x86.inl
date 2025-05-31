/// \file x86 SSE/AVX 3D vector implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "math/math.h"
#include "math/vec3.h"

Vec3f Vec3f::operator+(const Vec3f& other)
{
	return Vec3f(_mm_add_ps(v, other.v));
}

Vec3f Vec3f::operator-(const Vec3f& other)
{
	return Vec3f(_mm_sub_ps(v, other.v));
}

Vec3f Vec3f::operator*(float other)
{
	return Vec3f(_mm_mul_ps(v, _mm_set1_ps(other)));
}

Vec3f Vec3f::operator/(float other)
{
	return Vec3f(_mm_div_ps(v, _mm_set1_ps(other)));
}

FORCEINLINE f32 SumElems(v128 x)
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
	return _mm_cvtss_f32(sums);
}

f32 Vec3f::Length()
{
	v128 v2 = _mm_mul_ps(v, v);
	return Sqrt(SumElems(v2));
}

void Vec3f::Normalize()
{
	*this /= Length();
}

f32 Vec3f::Dot(const Vec3f& other)
{
	const v128 r = _mm_mul_ps(v, other.v);
	return SumElems(r);
}

Vec3f Vec3f::Cross(const Vec3f& other)
{
#ifdef CH_KNOWN_SIMD256
	// https://geometrian.com/resources/cross_product/
	const v128 t0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1));
	const v128 t1 = _mm_shuffle_ps(other.v, other.v, _MM_SHUFFLE(3, 1, 0, 2));
	const v128 t2 = _mm_mul_ps(t0, other.v);
	const v128 t4 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(3, 0, 2, 1));
	return Vec3f(_mm_fmsub_ps(t0, t1, t4));
#else
	const v128 t0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1));
	const v128 t1 = _mm_shuffle_ps(other.v, other.v, _MM_SHUFFLE(3, 1, 0, 2));
	const v128 t2 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 1, 0, 2));
	const v128 t3 = _mm_shuffle_ps(other.v, other.v, _MM_SHUFFLE(3, 0, 2, 1));
	return Vec3f(_mm_sub_ps(_mm_mul_ps(t0, t1), _mm_mul_ps(t2, t3)));
#endif
}
