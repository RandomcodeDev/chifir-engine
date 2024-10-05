// Type definitions

#pragma once

#include <cstdarg>
#include <cstdint>
#include <limits>
#if !defined CH_WIN32 || _MSC_VER >= 1400
#include <typeinfo>
#else
#include <typeinfo.h>
#endif

#include "compiler.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uptr;

// Avoid using in cases where underflow could happen
typedef uptr usize;
static_assert(sizeof(usize) == sizeof(size_t), "sizeof(usize) != sizeof(size_t)");

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef intptr_t sptr;
typedef sptr ssize;

// These two are in POSIX, so no use redefining them
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
typedef __m256 v256;
#elif defined CH_XBOX360
typedef __vector4 v128;
#elif defined CH_ARM64
typedef __n128 v128;
typedef __n128x2 v256;
#endif

typedef char* dstr;
typedef const char* cstr;

typedef wchar_t* dwstr;
typedef const wchar_t* cwstr;
