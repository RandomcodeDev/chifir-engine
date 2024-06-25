#include "base/base.h"

void* __cdecl operator new(usize size)
{
	return Base_Alloc(size);
}

void __cdecl operator delete(void* block, usize size)
{
	Base_Free(block);
}
