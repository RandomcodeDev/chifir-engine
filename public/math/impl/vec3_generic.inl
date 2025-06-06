/// \file Generic vector implementation, free for the compiler to optimize
/// \copyright 2025 Randomcode Developers

#pragma once

#include "math/math.h"
#include "math/vec3.h"

Vec3f Vec3f::operator+(const Vec3f& other)
{
	return Vec3f(x + other.x, y + other.y, z + other.z);
}

Vec3f Vec3f::operator-(const Vec3f& other)
{
	return Vec3f(x - other.x, y - other.y, z - other.z);
}

Vec3f Vec3f::operator*(float other)
{
	return Vec3f(x * other, y * other, z * other);
}

Vec3f Vec3f::operator/(float other)
{
	return Vec3f(x / other, y / other, z / other);
}

f32 Vec3f::Length()
{
	return Sqrt(x * x + y * y + z * z);
}

void Vec3f::Normalize()
{
	*this /= Length();
}

f32 Vec3f::Dot(const Vec3f& other)
{
	return x * other.x + y * other.y + z * other.z;
}

Vec3f Vec3f::Cross(const Vec3f& other)
{
    return Vec3f(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}
