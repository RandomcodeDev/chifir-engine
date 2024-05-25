#include "Memory.h"

void* operator new(usize size)
{
	return AllocateMemory(size);
}

void operator delete(void* block)
{
	FreeMemory(block);
}
