#include "base.h"
#include "base/base.h"
#include "base/dll.h"
#include "base/types.h"

MemoryInfo_t g_memInfo;

struct AllocInfo_t
{
	usize size;
	bool isFree;
	LinkedNode_t<SystemAllocation_t>* systemAllocation;
};

static CLinkedList<AllocInfo_t> s_allocations;

static s32 FindSystemNode(SystemAllocation_t* alloc, void* data)
{
	if (alloc->size - alloc->used >= (usize)data + sizeof(AllocInfo_t))
	{
		return 0;
	}

	return 1;
}

static LinkedNode_t<AllocInfo_t>* MakeNewNode(usize size)
{
	LinkedNode_t<SystemAllocation_t>* node = g_memInfo.allocations.Find(FindSystemNode, reinterpret_cast<void*>(size));
	if (!node)
	{
		ASSERT(Base_GetSystemMemory(size));
		node = g_memInfo.allocations.GetTail();
	}

	LinkedNode_t<AllocInfo_t>* alloc =
		reinterpret_cast<LinkedNode_t<AllocInfo_t>*>((static_cast<u8*>(node->data.memory) + node->data.used));
	alloc->data.size = size;
	alloc->data.isFree = false;
	alloc->data.systemAllocation = node;

	node->data.used += alloc->data.size + sizeof(AllocInfo_t);

	s_allocations.Append(alloc);

	return alloc;
}

static s32 FindFreeNode(AllocInfo_t* alloc, void* data)
{
	if (alloc->isFree && (alloc->size >= (usize)data))
	{
		return 0;
	}

	return 1;
}

static LinkedNode_t<AllocInfo_t>* GetFreeNode(usize size)
{
	LinkedNode_t<AllocInfo_t>* alloc = s_allocations.Find(FindFreeNode, reinterpret_cast<void*>(size));
	if (!alloc)
	{
		// Allocate 64 MB if not enough space for this
		alloc = MakeNewNode(64 * 1024 * 1024);
		return alloc; // Can skip next part because the node will be the exact size
	}

	// Free the rest of the block
	if (alloc->data.size - size >= sizeof(AllocInfo_t) + 1)
	{
		LinkedNode_t<AllocInfo_t>* next =
			reinterpret_cast<LinkedNode_t<AllocInfo_t>*>(reinterpret_cast<u8*>(alloc + 1) + alloc->data.size);
		next->data.size = alloc->data.size - sizeof(AllocInfo_t) - size;
		next->data.isFree = true;
		alloc->data.size = size;
		// Keep sorted
		s_allocations.InsertAfter(alloc, next);
	}

	return alloc;
}

static LinkedNode_t<AllocInfo_t>* GetNode(void* block)
{
	return static_cast<LinkedNode_t<AllocInfo_t>*>(block) - 1;
}

static void CoalesceAllocations()
{
	if (s_allocations.Size() > 1)
	{
		for (LinkedNode_t<AllocInfo_t>* alloc = s_allocations.GetHead(); !alloc->GetNext()->IsTail(); alloc = alloc->GetNext())
		{
			LinkedNode_t<AllocInfo_t>* next = alloc->GetNext();
			if (next->data.isFree)
			{
				alloc->data.size += next->data.size + sizeof(AllocInfo_t);
				s_allocations.Remove(next);
			}
		}
	}
}

BASEAPI void* Base_Alloc(usize size)
{
	LinkedNode_t<AllocInfo_t>* alloc = GetFreeNode(size);
	return static_cast<void*>(alloc + 1);
}

BASEAPI void Base_Free(void* block)
{
	if (block)
	{
		GetNode(block)->data.isFree = true;
		CoalesceAllocations();
	}
}
