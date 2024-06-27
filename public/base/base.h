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
extern BASEAPI void Base_Init();

// Shut down platform stuff
extern BASEAPI void Base_Shutdown();

// Exit the process and optionally display an error.
// For platform specific code, error can be an NTSTATUS or POSIX errno value.
// Otherwise, use 0 for normal exit, 1 for a possibly relevant platform-specific error code and an error message.
extern BASEAPI NORETURN void Base_Quit(u32 error, cstr msg, ...);

// Quit if a condition isn't true, add a message and code
#define ASSERT_MSG_CODE(x, msg, code)                                                                                            \
	if (!(x))                                                                                                                    \
	{                                                                                                                            \
		Base_Quit(code, "Assertion " #x " failed: " #msg);                                                                       \
	}

// Quit if a condition isn't true, add a message
#define ASSERT_MSG(x, msg) ASSERT_MSG_CODE(x, msg, 1)

// Quit if a condition isn't true, set the code
#define ASSERT_CODE(x, code) ASSERT_MSG_CODE(x, x, code)

// Quit if a condition isn't true
#define ASSERT(x) ASSERT_MSG(x, x)

// Swap
template <typename T> static inline void Swap(T& left, T& right)
{
	T swap = left;
	left = right;
	right = swap;
}

// FNV-1a hash
extern BASEAPI u32 Base_Fnv1a32(const void* data, usize size);
extern BASEAPI u64 Base_Fnv1a64(const void* data, usize size);

// Copy memory
extern BASEAPI void* Base_MemCpy(void* RESTRICT dest, const void* RESTRICT src, usize size);

// Clear memory
extern BASEAPI void* Base_MemSet(void* dest, u32 value, usize size);

// Allocate memory
extern BASEAPI void* Base_Alloc(usize count, usize size);

extern BASEAPI void* Base_Alloc(usize size);

template <typename T> T* Base_Alloc(usize count)
{
	return (T*)Base_Alloc(count, sizeof(T));
}

// Free memory from Base_Alloc
extern BASEAPI void Base_Free(void* block);
