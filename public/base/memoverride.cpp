#include <cerrno>

#include "base.h"
#include "types.h"

extern "C"
{
	void* __cdecl malloc(usize size)
	{
		return Base_Alloc(static_cast<ssize>(static_cast<usize>(size) & SSIZE_MAX));
	}

	void __cdecl free(void* block)
	{
		Base_Free(block);
	}

	void* __cdecl calloc(usize count, usize size)
	{
		return Base_Alloc(count * size);
	}

	void* __cdecl realloc(void* block, usize newSize)
	{
		return Base_Realloc(block, static_cast<ssize>(static_cast<usize>(newSize) & SSIZE_MAX));
	}

	void* __cdecl aligned_alloc(usize alignment, usize size)
	{
		return Base_Alloc(static_cast<ssize>(static_cast<usize>(size) & SSIZE_MAX), alignment);
	}

	usize __cdecl malloc_usable_size(void* block)
	{
		return Base_GetAllocSize(block);
	}

	void* memalign(usize alignment, usize size)
	{
		return aligned_alloc(alignment, size);
	}

	int posix_memalign(void** memptr, usize alignment, usize size)
	{
		*memptr = memalign(alignment, size);
		if (!*memptr)
		{
			return ENOMEM;
		}

		return 0;
	}
}

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
