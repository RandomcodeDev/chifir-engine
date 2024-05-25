#include "framework/Memory.h"

FRAMEWORK_API void* AllocateMemory(size_t size)
{
#ifdef KR_USE_MIMALLOC
	return mi_calloc(1, size);
#else
	return calloc(1, size);
#endif
}

FRAMEWORK_API void FreeMemory(void* memory)
{
#ifdef KR_USE_MIMALLOC
	return mi_free(memory);
#else
	return free(memory);
#endif
}
