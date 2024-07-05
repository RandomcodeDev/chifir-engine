// Type definitions

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#if _MSC_VER >= 1400
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
typedef uptr usize;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef intptr_t sptr;
typedef sptr ssize;

typedef float f32;
typedef double f64;
#ifdef CH_X86
typedef __m128 v128;
typedef __m256 v256;
#endif

// Note: "wide" and "narrow" refer, generally, to UTF-16 and UTF-8, not just whatever the platform decides.
// On platforms where wchar_t would be 32 bits, -fshort-wchar is used.

typedef char* dstr;
typedef const char* cstr;

typedef wchar_t* dwstr;
typedef const wchar_t* cwstr;
