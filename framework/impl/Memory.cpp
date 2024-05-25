#include "framework/Memory.h"

#ifdef KR_USE_MIMALLOC
#include "mimalloc.h"
#endif

FRAMEWORK_API void* AllocateMemory(usize size)
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
