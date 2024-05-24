#include "Memory.h"

void* operator new(size_t size)
{
	return AllocateMemory(size);
}

void operator delete(void* block)
{
	FreeMemory(block);
}
