/// malloc/free override
/// Copyright 2025 Randomcode Developers

#include <cerrno>

#include "base.h"
#include "types.h"

#ifdef _MSC_VER
#define RESTRICT_FUNC ATTRIBUTE(restrict)
#else
#define RESTRICT_FUNC
#endif

#ifdef CH_UNIX
#define VISIBLE DLLEXPORT
#else
#define VISIBLE
#endif

extern "C"
{
	VISIBLE RESTRICT_FUNC void* __cdecl malloc(usize size)
	{
		return Base_Alloc(static_cast<ssize>(size & SSIZE_MAX));
	}

	VISIBLE void __cdecl free(void* block)
	{
		Base_Free(block);
	}

	VISIBLE RESTRICT_FUNC void* __cdecl calloc(usize count, usize size)
	{
		return Base_Alloc(static_cast<ssize>(count * size));
	}

	VISIBLE RESTRICT_FUNC void* __cdecl realloc(void* block, usize newSize)
	{
		return Base_Realloc(block, static_cast<ssize>(static_cast<usize>(newSize) & SSIZE_MAX));
	}

	VISIBLE void* __cdecl aligned_alloc(usize alignment, usize size)
	{
		return Base_Alloc(static_cast<ssize>(size & SSIZE_MAX), static_cast<ssize>(alignment));
	}

#ifdef _MSC_VER
	RESTRICT_FUNC ATTRIBUTE(noalias) void* __cdecl _aligned_malloc(usize size, usize alignment)
	{
		return aligned_alloc(alignment, size);
	}
#endif

	VISIBLE usize __cdecl malloc_usable_size(void* block)
	{
		return static_cast<usize>(Base_GetAllocSize(block));
	}

	VISIBLE void* memalign(usize alignment, usize size)
	{
		return aligned_alloc(alignment, size);
	}

	VISIBLE int posix_memalign(void** memptr, usize alignment, usize size)
	{
		*memptr = memalign(alignment, size);
		if (!*memptr)
		{
			return ENOMEM;
		}

		return 0;
	}
}

#if defined _MSC_VER && defined IN_BASE
EXPORT_CDECL(malloc)
EXPORT_CDECL(free)
EXPORT_CDECL(calloc)
EXPORT_CDECL(realloc)
EXPORT_CDECL(aligned_alloc)
EXPORT_CDECL(_aligned_malloc)
#endif

void* __cdecl operator new(usize size)
{
	void* block = Base_Alloc(static_cast<ssize>(static_cast<usize>(size) & SSIZE_MAX));
	// new isn't allowed to return nullptr
	ASSERT(block != nullptr);
	return block;
}

void* __cdecl operator new[](usize size)
{
	return operator new(static_cast<usize>(size) & SSIZE_MAX);
}

void __cdecl operator delete(void* block) noexcept
{
	Base_Free(block);
}

void __cdecl operator delete(void* block, usize size) noexcept
{
	UNUSED(size);
	operator delete(block);
}

void __cdecl operator delete[](void* block) noexcept
{
	operator delete(block);
}

void __cdecl operator delete[](void* block, usize size) noexcept
{
	UNUSED(size);
	operator delete[](block);
}
