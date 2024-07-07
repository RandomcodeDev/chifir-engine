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
#define ASSERT_MSG_CODE(x, msg, code)                                                                                            \
	if (!(x))                                                                                                                    \
	{                                                                                                                            \
		Base_Quit(code, "Assertion " #x " failed: " #msg);                                                                       \
	}

// Quit if a condition isn't true, add a message
#define ASSERT_MSG(x, msg) ASSERT_MSG_CODE(x, msg, 1)

// Quit if a condition isn't true, set the code
#define ASSERT_CODE(x, code) ASSERT_MSG_CODE(x, code, code)

// Quit if a condition isn't true
#define ASSERT(x) ASSERT_MSG(x,)

// Round val up to a multiple of align (align must be a power of two)
#define ALIGN(val, align) (((val) + (align) - 1) & ~((align) - 1))

// Swap
template <typename T> static inline void Swap(T& left, T& right)
{
	T swap = left;
	left = right;
	right = swap;
}

// FNV-1a hash
BASEAPI u32 Base_Fnv1a32(const void* data, usize size);
BASEAPI u64 Base_Fnv1a64(const void* data, usize size);

// Copy memory
BASEAPI void* Base_MemCpy(void* RESTRICT dest, const void* RESTRICT src, usize size);

// Clear memory
BASEAPI void* Base_MemSet(void* dest, u32 value, usize size);

// Allocate memory
BASEAPI void* Base_Alloc(usize size);

static inline void* Base_Alloc(usize count, usize size)
{
	return Base_Alloc(count * size);
}

template <typename T> static inline T* Base_Alloc(usize count)
{
	return (T*)Base_Alloc(count, sizeof(T));
}

// Free memory from Base_Alloc
BASEAPI void Base_Free(void* block);
