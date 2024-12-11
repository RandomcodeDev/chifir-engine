// Base definitions

#pragma once

#include "compiler.h"
#include "dll.h"
#include "macros.h"
#include "types.h"

#ifdef CH_STATIC
#define BASEAPI
#else
#ifdef IN_BASE
#define BASEAPI DLLEXPORT
#else
#define BASEAPI DLLIMPORT
#endif
#endif

class CString;

typedef struct DateTime
{
	u32 year;
	u32 month; // 1-12
	u32 day; // 1-31

	u32 hour; // 0-23
	u32 minute; // 0-59
	u32 second; // 0-59
	u32 millisecond; // not guaranteed, 0-999
	u32 weekDay; // 0-6, sunday to saturday
	
    DateTime()
	{
        extern BASEAPI void Plat_GetDateTime(DateTime&, bool = false);
		Plat_GetDateTime(*this);
	}
} DateTime_t;

// Initialize platform stuff, detect CPU features, and other stuff
extern BASEAPI void Base_Init();

// Shut down platform stuff
extern BASEAPI void Base_Shutdown();

// Exit the process and optionally display an error.
// For platform specific code, error can be an NTSTATUS or POSIX errno value.
// Otherwise, use 0 for normal exit, 1 for a possibly relevant platform-specific error code (such as NtLastError()).
extern BASEAPI NORETURN void Base_Abort(s32 error, cstr msg, ...);

// This is for when Base_Abort might have side effects that could lead to recursion, such as calling Base_VFormat
extern BASEAPI NORETURN void Base_AbortSafe(s32 error, cstr msg);

// Quit and log it
extern BASEAPI NORETURN void Base_Quit(cstr msg, ...);

// Swap
template <typename T> FORCEINLINE void Swap(T& left, T& right)
{
	T swap = left;
	left = right;
	right = swap;
}

// Maximum of two values
template <typename T> FORCEINLINE T Max(const T& left, const T& right)
{
	return left > right ? left : right;
}

// Minimum of two values
template <typename T> FORCEINLINE T Min(const T& left, const T& right)
{
	return left < right ? left : right;
}

// Clamp a value within range
template <typename T> FORCEINLINE T Clamp(const T& value, const T& min, const T& max)
{
	return Max(min, Min(value, max));
}

// FNV-1a hash
extern BASEAPI u32 Base_Fnv1a32(const void* data, ssize size);
extern BASEAPI u64 Base_Fnv1a64(const void* data, ssize size);

// Copy memory
extern BASEAPI void* Base_MemCopy(void* RESTRICT dest, const void* RESTRICT src, ssize size);

// Compare memory
extern BASEAPI s32 Base_MemCompare(const void* RESTRICT a, const void* RESTRICT b, ssize size);

// Clear memory
extern BASEAPI void* Base_MemSet(void* dest, u32 value, ssize size);

// Find a byte in a block of memory
extern BASEAPI ssize Base_MemFind(const void* data, ssize size, u8 value, bool reverse = false);

// Find a sequence of bytes in a block of memory
extern BASEAPI ssize Base_MemFind(const void* RESTRICT data, ssize size, const void* RESTRICT sequence, ssize sequenceSize, bool reverse = false);

// Allocate aligned memory
extern BASEAPI ALLOCATOR void* Base_Alloc(ssize size, ssize alignment = 8);

// Allocate an array of T with count elements, needs to be initialized with placement new
// if constructors matter.
template <typename T> FORCEINLINE T* Base_Alloc(ssize count)
{
	return reinterpret_cast<T*>(Base_Alloc(count * SIZEOF(T), ALIGNOF(T)));
}

// Resize an allocation. block == nullptr is equivalent to Base_Alloc, newSize == 0 is equivalent to Base_Free.
extern BASEAPI void* Base_Realloc(void* block, ssize newSize);

// Free memory from Base_Alloc
extern BASEAPI void Base_Free(void* block);

// Get the size of an allocation
extern BASEAPI ssize Base_GetAllocSize(void* block);

// Get the alignment of an allocation
extern BASEAPI ssize Base_GetAllocAlignment(void* block);
