/// Compiler-specific defines.
//
/// In general, anything non-standard should be behind a common macro, so that if in the future some compiler doesn't have
/// it, it can be replaced more easily.

#pragma once

#include <cstdarg>

#include "macros.h"

#ifdef __clang__
#define COMPILER "Clang " __clang_version__
#elif defined _MSC_VER
#define MSVC_STRING_VER(ver) #ver
#define MSVC_STRING_VER2(ver) MSVC_STRING_VER(ver)
#define COMPILER "MSVC " MSVC_STRING_VER2(_MSC_FULL_VER)
#endif

#ifdef CH_ARM64
#include <arm_neon.h>
#endif

#ifdef _MSC_VER
#ifdef CH_XBOX360
#include <VectorIntrinsics.h>
#endif

#ifdef CH_X86
#ifdef __clang__
#include <x86intrin.h>
#else
#include <intrin.h>
#endif
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
/// MSVC likes to be told when an intrinsic function is defined
#define DEFINE_INTRINSIC(x) __pragma(function(x))

#define OPTIMIZE_OFF _Pragma("optimize(\"\", off)")
#define OPTIMIZE_ON _Pragma("optimize(\"\", on)")

/// Get the return address of the current function
#define Plat_GetReturnAddress() reinterpret_cast<uptr>(_ReturnAddress())

#define BYTESWAP16(x) _byteswap_ushort(x)
#define BYTESWAP32(x) _byteswap_ulong(x)
#define BYTESWAP64(x) _byteswap_uint64(x)

/// Initialize the stack security cookie, local to the current DLL/EXE
extern "C" void __cdecl __security_init_cookie();

/// Call global constructors, local to the current DLL/EXE
extern void RunGlobalConstructors();

/// Call global destructors, local to the current DLL/EXE
extern void RunGlobalDestructors();
#else
#ifdef CH_X86
#include <cpuid.h>
#include <immintrin.h>
#include <x86intrin.h>
#endif

#define ATTRIBUTE(x) __attribute__((x))
#define ALIAS(a, b)  extern "C" TYPEOF(a) ATTRIBUTE(alias(#a)) b;
#define BREAKPOINT() __builtin_trap()
#define ASSUME(x)    __builtin_assume(x)
#define DEFINE_INTRINSIC(x)

#define OPTIMIZE_OFF _Pragma("clang optimize off")
#define OPTIMIZE_ON _Pragma("clang optimize on")

/// Get the return address of the current function
#define Plat_GetReturnAddress() reinterpret_cast<uptr>(__builtin_return_address(0))

#define BYTESWAP16(x) __builtin_bswap16(x)
#define BYTESWAP32(x) __builtin_bswap32(x)
#define BYTESWAP64(x) __builtin_bswap64(x)
#endif

#if __cplusplus < 201100L
/// Because = default is invalid in C++03
#define DEFAULT                                                                                                                  \
	{                                                                                                                            \
	}
#else
/// Because = default is invalid in C++03
#define DEFAULT = default
#endif

/// MSVC and Clang/GCC understand all of these equally

#define TYPEOF(x) __typeof__(x)
#define NORETURN  ATTRIBUTE(noreturn)
#if _MSC_VER <= 1600
#define RESTRICT
#else
#define RESTRICT __restrict
#endif
#define ALIGNOF(x)  __alignof(x)
#define FORCEINLINE __forceinline

/// Decorated function name
#if defined _MSC_VER
#define FUNCTION_NAME __FUNCSIG__
#else
#define FUNCTION_NAME __PRETTY_FUNCTION__
#endif

/// Xbox 360 doesn't define this
#ifndef va_copy
#define va_copy(dest, source) (dest) = (source)
#endif

#if defined CH_WIN32 && _MSC_VER >= 1700
#define ALLOCATOR ATTRIBUTE(allocator)
#else
#define ALLOCATOR
#endif

#define ALIGNED(x) ATTRIBUTE(aligned(x))

#ifdef CH_LITTLE_ENDIAN
#define LITTLE_ENDIAN_TO_NATIVE_16(x) (x)
#define LITTLE_ENDIAN_TO_NATIVE_32(x) (x)
#define LITTLE_ENDIAN_TO_NATIVE_64(x) (x)

#define NATIVE_TO_LITTLE_ENDIAN_16(x) (x)
#define NATIVE_TO_LITTLE_ENDIAN_32(x) (x)
#define NATIVE_TO_LITTLE_ENDIAN_64(x) (x)

#define BIG_ENDIAN_TO_NATIVE_16(x) BYTESWAP16(x)
#define BIG_ENDIAN_TO_NATIVE_32(x) BYTESWAP32(x)
#define BIG_ENDIAN_TO_NATIVE_64(x) BYTESWAP64(x)

#define NATIVE_TO_BIG_ENDIAN_16(x) BYTESWAP16(x)
#define NATIVE_TO_BIG_ENDIAN_32(x) BYTESWAP32(x)
#define NATIVE_TO_BIG_ENDIAN_64(x) BYTESWAP64(x)
#elif CH_BIG_ENDIAN
#define LITTLE_ENDIAN_TO_NATIVE_16(x) BYTESWAP16(x)
#define LITTLE_ENDIAN_TO_NATIVE_32(x) BYTESWAP32(x)
#define LITTLE_ENDIAN_TO_NATIVE_64(x) BYTESWAP64(x)

#define NATIVE_TO_LITTLE_ENDIAN_16(x) BYTESWAP16(x)
#define NATIVE_TO_LITTLE_ENDIAN_32(x) BYTESWAP32(x)
#define NATIVE_TO_LITTLE_ENDIAN_64(x) BYTESWAP64(x)

#define BIG_ENDIAN_TO_NATIVE_16(x) (x)
#define BIG_ENDIAN_TO_NATIVE_32(x) (x)
#define BIG_ENDIAN_TO_NATIVE_64(x) (x)

#define NATIVE_TO_BIG_ENDIAN_16(x) (x)
#define NATIVE_TO_BIG_ENDIAN_32(x) (x)
#define NATIVE_TO_BIG_ENDIAN_64(x) (x)
#else
#error "Unknown endianness"
#endif
