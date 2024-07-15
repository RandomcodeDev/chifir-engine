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

// This is for when Base_Quit might have side effects that could lead to recursion, such as calling Base_VFormat
BASEAPI NORETURN void Base_QuitSafe(s32 error, cstr msg);

#define ASSERT_IMPL(cond, action)                                                                                                \
	if (!(cond))                                                                                                                 \
	{                                                                                                                            \
		action;                                                                                                                  \
	}

// Quit if a condition isn't true, add a message
#define ASSERT_MSG(cond, ...) ASSERT_IMPL(cond, Base_Quit(1, "Assertion " #cond " failed: " __VA_ARGS__))

// Quit if a condition isn't true, add a message
#define ASSERT_MSG_SAFE(cond, msg) ASSERT_IMPL(cond, Base_QuitSafe(1, "Assertion " #cond " failed: " msg))

// Quit if a condition isn't true
#define ASSERT(cond) ASSERT_IMPL(cond, Base_QuitSafe(1, "Assertion " #cond " failed"))

// Round val up to a multiple of align (align must be a power of two)
#define ALIGN(val, align) (((val) + (align)-1) & ~((align)-1))

// Get the number of elements in a stack/static array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Stringize something
#define STRINGIZE(x) #x

// Stringize something, and expand macros one level
#define STRINGIZE_EXPAND(x) STRINGIZE(x)

// Get the address of a structure containing the given pointer
#define CONTAINING_STRUCTURE(type, member, ptr) (reinterpret_cast<type*>(reinterpret_cast<uptr>(ptr) - offsetof(type, member)))

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
BASEAPI u32 Base_Fnv1a32(const void* data, ssize size);
BASEAPI u64 Base_Fnv1a64(const void* data, ssize size);

// Copy memory
BASEAPI void* Base_MemCopy(void* RESTRICT dest, const void* RESTRICT src, ssize size);

// Compare memory
BASEAPI s32 Base_MemCompare(const void* RESTRICT a, const void* RESTRICT b, ssize size);

// Clear memory
BASEAPI void* Base_MemSet(void* dest, u32 value, ssize size);

// Allocate aligned memory
BASEAPI void* Base_Alloc(ssize size, ssize alignment = 8);

// Allocate an array of T with count elements
template <typename T> static inline T* Base_Alloc(ssize count)
{
	return reinterpret_cast<T*>(Base_Alloc(count * sizeof(T), ALIGNOF(T)));
}

// Resize an allocation. block == nullptr is equivalent to Base_Alloc, newSize == 0 is equivalent to Base_Free.
BASEAPI void* Base_Realloc(void* block, ssize newSize);

// Free memory from Base_Alloc
BASEAPI void Base_Free(void* block);
