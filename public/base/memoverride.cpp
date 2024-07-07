#include "base/base.h"
#include "base/types.h"

void* __cdecl operator new(usize size)
{
	return Base_Alloc(size);
}

void __cdecl operator delete(void* block)
{
	Base_Free(block);
}

void __cdecl operator delete(void* block, usize size)
{
	(void)size;
	operator delete(block);
}
