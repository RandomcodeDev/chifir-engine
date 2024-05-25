#include "Memory.h"

FRAMEWORK_API void* AllocateMemory(size_t size)
{
	return mi_calloc(1, size);
}

FRAMEWORK_API void FreeMemory(void* memory)
{
	return mi_free(memory);
}
