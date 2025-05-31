/// \file 3D vector type
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

struct Vec3f
{
	union {
		// TODO: are there other useful names for these?
		struct
		{
			f32 x;
			f32 y;
			f32 z;
		};
		float raw[3];
#ifdef CH_KNOWN_SIMD128
		v128 v;
#endif
	};

	// TODO: is loading this with SIMD faster?
	Vec3f(f32 all) : x(all), y(all), z(all)
	{
	}

	Vec3f(f32 x, f32 y, f32 z) : x(x), y(y), z(z)
	{
	}

#ifdef CH_KNOWN_SIMD128
	Vec3f(v128 v) : v(v)
	{
	}
#endif

	f32 operator[](ssize index)
	{
		return raw[index];
	}

	Vec3f operator+(const Vec3f& other);
	void operator+=(const Vec3f& other)
	{
		*this = *this + other;
	}

	Vec3f operator-(const Vec3f& other);
	void operator-=(const Vec3f& other)
	{
		*this = *this - other;
	}

	Vec3f operator*(float other);
	void operator*=(float other)
	{
		*this = *this * other;
	}

	Vec3f operator/(float other);
	void operator/=(float other)
	{
		*this = *this / other;
	}

	f32 Length();
	void Normalize();
	f32 Dot(const Vec3f& other);
	Vec3f Cross(const Vec3f& other);
};

#ifdef CH_KNOWN_SIMD128
#ifdef CH_X86
#include "impl/vec3_x86.inl"
#endif
#else
#include "impl/vec3_generic.inl"
#endif
