/// \file Various types of allocators
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base.h"
#include "compiler.h"
#include "types.h"

/// Generic allocator
class BASEAPI IAllocator
{
  public:
	virtual ~IAllocator() = default;

	/// Allocate memory
	virtual void* Alloc(ssize size) = 0;

	/// Allocate an array of objects
	template <typename T> T* Alloc(ssize count)
	{
		return reinterpret_cast<T*>(Alloc(SIZEOF(T) * count));
	}

	/// Free an allocation
	virtual void Free(void* block, ssize originalSize) = 0;
};

/// Arena allocator
class BASEAPI CArenaAllocator: public IAllocator
{
  public:
	CArenaAllocator(void* arena, ssize size) : m_arena(), m_arenaSize(size), m_arenaOffset(0), m_ownMemory(false)
	{
	}
	CArenaAllocator(ssize size) : CArenaAllocator(new u8[size], size)
	{
		m_ownMemory = true;
	}
	~CArenaAllocator()
	{
		if (m_ownMemory)
		{
			delete[] m_arena;
		}
	}

	/// Allocate memory
	virtual void* Alloc(ssize size);

	/// Free a previous allocation, must be done in reverse order of allocations.
	/// Usually better to throw out the whole allocator.
	virtual void Free(void* block, ssize size);

	/// Get the base of the arena
	void* GetBase()
	{
		return m_arena;
	}

	/// Get the size of the arena
	ssize GetSize()
	{
		return m_arenaSize;
	}

	/// Get the current end offset
	ssize GetEndOffset()
	{
		return m_arenaOffset;
	}

  private:
	u8* m_arena;
	ssize m_arenaSize;
	ssize m_arenaOffset;
	bool m_ownMemory;
};
