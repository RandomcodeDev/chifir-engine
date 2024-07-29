// Compiler-specific defines.
//
// In general, anything non-standard should be behind a common macro, so that if in the future some compiler doesn't have
// it, it can be replaced more easily.

#pragma once

#include <cstdarg>

#ifdef CH_ARM64
#include <arm_neon.h>
#endif

#if defined _MSC_VER
#ifdef CH_XBOX360
#include <VectorIntrinsics.h>
#elif defined CH_X86
#include <intrin.h>
#endif

#define ATTRIBUTE(x)                  __declspec(x)
#define EXPORT_RAW(x)                 __pragma(comment(linker, "/export:" x))
#define EXPORT(x)                     EXPORT_RAW(#x)
#define EXPORT_AS_RAW(orig, alt)      EXPORT_RAW(alt "=" orig)
#define EXPORT_CURRENT_FUNCTION_AS(x) EXPORT_AS_RAW(__FUNCTION__, x)
#define EXPORT_AS(a, b)               EXPORT_AS_RAW(#a, #b)
#define ALIAS_RAW(a, b)               __pragma(comment(linker, "/alternatename:" b "=" a))
#define ALIAS(a, b)                   ALIAS_RAW(#a, #b)
#define BREAKPOINT()                  __debugbreak()
#define ASSUME(x)                     __assume(x)
// MSVC likes to be told when an intrinsic function is defined
#define DEFINE_INTRINSIC(x) __pragma(function(x))

// Initialize the stack security cookie
extern "C" void __cdecl __security_init_cookie();
#else
#define ATTRIBUTE(x) __attribute__((x))
#define ALIAS(a, b)  extern "C" TYPEOF(a) ATTRIBUTE(alias(#a)) b;
#define BREAKPOINT() __builtin_trap()
#define ASSUME(x)    __builtin_assume(x)
#define DEFINE_INTRINSIC(x)
#endif

#if __cplusplus < 201100L
// Because = default is invalid in C++03
#define DEFAULT                                                                                                                  \
	{                                                                                                                            \
	}
#else
// Because = default is invalid in C++03
#define DEFAULT = default
#endif

// MSVC and Clang/GCC understand all of these equally

#define TYPEOF(x) __typeof__(x)
#define NORETURN  ATTRIBUTE(noreturn)
#if _MSC_VER <= 1600
#define RESTRICT
#else
#define RESTRICT __restrict
#endif
#define ALIGNOF(x)  __alignof(x)
#define FORCEINLINE __forceinline

// Decorated function name
#if _MSC_VER <= 1600
#define FUNCTION_NAME __FUNCDNAME__
#else
#define FUNCTION_NAME __PRETTY_FUNCTION__
#endif

// Xbox 360 doesn't define this
#ifndef va_copy
#define va_copy(dest, source) (dest) = (source)
#endif

#if defined CH_WIN32 && _MSC_VER >= 1700
#define ALLOCATOR ATTRIBUTE(allocator)
#else
#define ALLOCATOR
#endif
