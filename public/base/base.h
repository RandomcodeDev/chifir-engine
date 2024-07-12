// Base definitions

#pragma once

#include "dll.h"
#include "types.h"

#ifdef IN_BASE
#define BASEAPI DLLEXPORT
#else
#define BASEAPI DLLIMPORT
#endif

// Initialize platform stuff, detect CPU features, and other stuff
BASEAPI void Base_Init();

// Shut down platform stuff
BASEAPI void Base_Shutdown();

// Exit the process and optionally display an error.
// For platform specific code, error can be an NTSTATUS or POSIX errno value.
// Otherwise, use 0 for normal exit, 1 for a possibly relevant platform-specific error code and an error message.
BASEAPI NORETURN void Base_Quit(s32 error, cstr msg, ...);

// Quit if a condition isn't true, add a message and code
#define ASSERT_MSG_CODE(cond, code, ...)                                                                                         \
	if (!(cond))                                                                                                                 \
	{                                                                                                                            \
		Base_Quit(code, "Assertion " #cond " failed: " __VA_ARGS__);                                                             \
	}

// Quit if a condition isn't true, add a message
#define ASSERT_MSG(cond, ...) ASSERT_MSG_CODE(cond, 1, __VA_ARGS__)

// Quit if a condition isn't true, set the exit code
#define ASSERT_CODE(cond, code) ASSERT_MSG_CODE(cond, code, "error " #code)

// Quit if a condition isn't true
#define ASSERT(cond) ASSERT_MSG(cond, ) // the msg parameter is intentionally left blank

// Round val up to a multiple of align (align must be a power of two)
#define ALIGN(val, align) (((val) + (align)-1) & ~((align)-1))

// Get the number of elements in a stack/static array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Swap
template <typename T> static inline void Swap(T& left, T& right)
{
	T swap = left;
	left = right;
	right = swap;
}

// Maximum of two values
template <typename T> static inline T Max(const T& left, const T& right)
{
	return left > right ? left : right;
}

// Minimum of two values
template <typename T> static inline T Min(const T& left, const T& right)
{
	return left < right ? left : right;
}

// FNV-1a hash
BASEAPI u32 Base_Fnv1a32(const void* data, usize size);
BASEAPI u64 Base_Fnv1a64(const void* data, usize size);

// Copy memory
BASEAPI void* Base_MemCopy(void* RESTRICT dest, const void* RESTRICT src, usize size);

// Clear memory
BASEAPI void* Base_MemSet(void* dest, u32 value, usize size);

// Allocate aligned memory
BASEAPI void* Base_Alloc(usize size, usize alignment = 8);

// Allocate an array of T with count elements
template <typename T> static inline T* Base_Alloc(usize count)
{
	return reinterpret_cast<T*>(Base_Alloc(count * sizeof(T), alignof(T)));
}

// Resize an allocation
BASEAPI void* Base_Realloc(void* block, usize newSize);

// Free memory from Base_Alloc
BASEAPI void Base_Free(void* block);
