/// \file Allocator implementations
/// \copyright 2025 Randomcode Developers

#include "base/allocator.h"

void* CArenaAllocator::Alloc(ssize size)
{
	if (m_arenaOffset + size <= m_arenaSize)
	{
		void* ptr = m_arena + m_arenaOffset;
		m_arenaOffset += size;
		return ptr;
	}

	return nullptr;
}

void CArenaAllocator::Free(void* block, ssize size)
{
	m_arenaOffset -= size;
}
