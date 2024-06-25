// Clang stuff

#include "base/base.h"
#include "base/types.h"

extern "C"
{
	void* memcpy(void* RESTRICT dest, const void* RESTRICT src, usize size)
	{
		Base_MemCpy(dest, src, size);
		return dest;
	}

	void* memset(void* dest, s32 value, usize size)
	{
		Base_MemSet(dest, value, size);
		return dest;
	}

	// Presumably emitted for things like a pure virtual call
	void __std_terminate()
	{
		Base_Quit(0, "Invalid behaviour");
	}
}
