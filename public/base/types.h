/// \file Type definitions
/// \copyright 2025 Randomcode Developers

/// If you're a Rust fan, I'm sure you'll love these, except it's s32 instead of i32 (sorry)

#pragma once

#include <cstdarg>
#include <cstdint>
#include <limits>
#if defined _MSC_VER && _MSC_VER < 1400
#include <typeinfo.h>
#else
#include <typeinfo>
#endif

#include "compiler.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uptr;

/// Avoid using in cases where underflow could happen
typedef uptr usize;
static_assert(sizeof(usize) == sizeof(std::size_t), "error: sizeof(usize) != sizeof(size_t)");

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef intptr_t sptr;
typedef sptr ssize;

/// These two are in POSIX, so no use redefining them
#ifndef SSIZE_MIN
#define SSIZE_MIN INTPTR_MIN
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX INTPTR_MAX
#endif

#define SIZEOF(x) static_cast<ssize>(sizeof(x))

typedef float f32;
typedef double f64;
#ifdef CH_X86
typedef __m128 v128;
#ifndef CH_XBOX
typedef __m256 v256;
#endif
#elif defined CH_XENON
typedef __vector4 v128;
#elif defined CH_ARM64
typedef float32x2_t v128;
typedef float32x4_t v256;
#endif

// dynamic string
typedef char* dstr;

// constant string
typedef const char* cstr;

// dynamic wide string
typedef wchar_t* dwstr;

// constant wide string
typedef const wchar_t* cwstr;
