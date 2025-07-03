/// \file Generic vector implementation, free for the compiler to optimize
/// \copyright 2025 Randomcode Developers

#pragma once

#include "math/math.h"
#include "math/vec4.h"

Vec4f Vec4f::operator+(const Vec4f& other)
{
	return Vec4f(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vec4f Vec4f::operator-(const Vec4f& other)
{
	return Vec4f(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vec4f Vec4f::operator*(float other)
{
	return Vec4f(x * other, y * other, z * other, z * other);
}

Vec4f Vec4f::operator/(float other)
{
	return Vec4f(x / other, y / other, z / other, w / other);
}

f32 Vec4f::Length()
{
	return Sqrt(x * x + y * y + z * z + w * w);
}

void Vec4f::Normalize()
{
	*this /= Length();
}

f32 Vec4f::Dot(const Vec4f& other)
{
	return x * other.x + y * other.y + z * other.z + w * other.w;
}
