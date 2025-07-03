/// \file x86 SSE/AVX 4D vector implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "math/math.h"
#include "math/vec4.h"

Vec4f Vec4f::operator+(const Vec4f& other)
{
	return Vec4f(_mm_add_ps(v, other.v));
}

Vec4f Vec4f::operator-(const Vec4f& other)
{
	return Vec4f(_mm_sub_ps(v, other.v));
}

Vec4f Vec4f::operator*(float other)
{
	return Vec4f(_mm_mul_ps(v, _mm_set1_ps(other)));
}

Vec4f Vec4f::operator/(float other)
{
	return Vec4f(_mm_div_ps(v, _mm_set1_ps(other)));
}

f32 Vec4f::Length()
{
	v128 v2 = _mm_mul_ps(v, v);
	return Sqrt(Math_SumElems(v2));
}

void Vec4f::Normalize()
{
	*this /= Length();
}

f32 Vec4f::Dot(const Vec4f& other)
{
	const v128 r = _mm_mul_ps(v, other.v);
	return Math_SumElems(r);
}
