// libc functions that forward to Base functions for when libraries won't listen

#include "base.h"
#include "basicstr.h"
#include "types.h"

extern "C"
{
	// MSVC doesn't like these defined when /GL is in use
#if !defined _MSC_VER || (defined _MSC_VER && !defined CH_DEBUG)
	DEFINE_INTRINSIC(memcmp)
	BASEAPI s32 memcmp(const void* RESTRICT a, const void* RESTRICT b, usize size)
	{
		return Base_MemCompare(a, b, size);
	}

	DEFINE_INTRINSIC(memmove)
	BASEAPI void* memmove(void* RESTRICT dest, const void* RESTRICT src, usize size)
	{
		return Base_MemCopy(dest, src, size);
	}

	DEFINE_INTRINSIC(memcpy)
	BASEAPI void* memcpy(void* RESTRICT dest, const void* RESTRICT src, usize size)
	{
		return memmove(dest, src, size);
	}

	DEFINE_INTRINSIC(memset)
	BASEAPI void* memset(void* dest, s32 value, usize size)
	{
		return Base_MemSet(dest, value, size);
	}

	DEFINE_INTRINSIC(strcmp)
	BASEAPI s32 strcmp(cstr RESTRICT a, cstr RESTRICT b)
	{
		return Base_StrCompare(a, b);
	}

	DEFINE_INTRINSIC(strlen)
	BASEAPI usize strlen(cstr str)
	{
		return Base_StrLength(str);
	}
#endif
}

