#include "base.h"
#include "types.h"

void* __cdecl operator new(usize size)
{
	return Base_Alloc(static_cast<ssize>(Min(size, static_cast<usize>(SSIZE_MAX))));
}

void* __cdecl operator new[](usize size)
{
	return operator new(static_cast<ssize>(Min(size, static_cast<usize>(SSIZE_MAX))));
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
