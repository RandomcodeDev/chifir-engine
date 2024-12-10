#include "base.h"
#include "types.h"

void* __cdecl operator new(usize size)
{
	void* block = Base_Alloc(static_cast<ssize>(static_cast<usize>(size) & SSIZE_MAX));
	// new isn't allowed to return nullptr
	ASSERT(block != nullptr);
	return block;
}

void* __cdecl operator new[](usize size)
{
	return operator new(static_cast<ssize>(static_cast<usize>(size) & SSIZE_MAX));
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

void __cdecl operator delete[](void* block)
{
	operator delete(block);
}

void __cdecl operator delete[](void* block, usize size)
{
	(void)size;
	operator delete[](block);
}
