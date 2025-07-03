/// \file 4D vector type
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

struct Vec4f
{
	union {
		// TODO: are there other useful names for these?
		struct
		{
			f32 x;
			f32 y;
			f32 z;
            f32 w
		};
		float raw[4];
#ifdef CH_KNOWN_SIMD128
		v128 v;
#endif
	};

	// TODO: is loading this with SIMD faster?
	Vec4f(f32 all) : x(all), y(all), z(all), w(all)
	{
	}

	Vec4f(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w)
	{
	}

#ifdef CH_KNOWN_SIMD128
	Vec4f(v128 v) : v(v)
	{
	}
#endif

	f32 operator[](ssize index)
	{
		return raw[index];
	}

	Vec4f operator+(const Vec4f& other);
	void operator+=(const Vec4f& other)
	{
		*this = *this + other;
	}

	Vec4f operator-(const Vec4f& other);
	void operator-=(const Vec4f& other)
	{
		*this = *this - other;
	}

	Vec4f operator*(float other);
	void operator*=(float other)
	{
		*this = *this * other;
	}
    f32 operator*(const Vec4f& other)
    {
        return Dot(other);
    }

	Vec4f operator/(float other);
	void operator/=(float other)
	{
		*this = *this / other;
	}

	f32 Length();
	void Normalize();
	f32 Dot(const Vec4f& other);
};

#ifdef CH_KNOWN_SIMD128
#ifdef CH_X86
#include "impl/vec4_x86.inl"
#endif
#else
#include "impl/vec4_generic.inl"
#endif
