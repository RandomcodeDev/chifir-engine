// This is a basic general allocator that can serve as a basis for more specialized things like bump allocators. It's implemented
// on top of Base_GetSystemMemory, and uses a linked list of allocations, with the nodes as headers to chunks of memory. It still
// needs to be tested more thoroughly, and is missing logging for errors, thread safety, and proper handling of alignment. It also
// can't handle individual allocations larger than 64MB.

#include "base.h"
#include "base/base.h"
#include "base/dll.h"
#include "base/types.h"

MemoryInfo_t g_memInfo;

struct AllocInfo_t
{
	usize size; // Includes the size of the AllocNode_t header
	bool isFree;
	LinkedNode_t<SystemAllocation_t>* systemAllocation;
};

typedef LinkedNode_t<AllocInfo_t> AllocNode_t;

// List of allocations. Nodes are stored at the start of each chunk of memory in the allocator's control, and their size is
// included in AllocInfo_t's size member.
// TODO: once threading has been implemented, this MUST be made thread safe.
static CLinkedList<AllocInfo_t> s_allocations;

static s32 FindSystemNode(SystemAllocation_t* alloc, void* data)
{
	if (alloc->size - alloc->used >= (usize)data)
	{
		return 0;
	}

	return 1;
}

// Size of system nodes which are directly allocated with NtAllocateVirtualMemory/mmap
static const usize SYSTEM_ALLOC_SIZE = 64 * 1024 * 1024;

// This finds a system node with the requested size, or allocates a new one of a fixed size
static AllocNode_t* MakeNewNode(usize size)
{
	LinkedNode_t<SystemAllocation_t>* node = g_memInfo.allocations.Find(FindSystemNode, reinterpret_cast<void*>(size));
	if (!node)
	{
		ASSERT(Base_GetSystemMemory(SYSTEM_ALLOC_SIZE));
		node = g_memInfo.allocations.GetTail();
	}

	AllocNode_t* alloc = reinterpret_cast<AllocNode_t*>((static_cast<u8*>(node->data.memory) + node->data.used));
	alloc->data.size = size;
	alloc->data.isFree = false;
	alloc->data.systemAllocation = node;

	node->data.used += alloc->data.size;

	s_allocations.Append(alloc);

	return alloc;
}

static usize EffectiveAllocSize(AllocInfo_t* info)
{
	return info->size - sizeof(AllocNode_t);
}

static usize EffectiveSize(AllocNode_t* node)
{
	return EffectiveAllocSize(&node->data);
}

static s32 FindFreeNode(AllocInfo_t* alloc, void* data)
{
	if (alloc->isFree && (EffectiveAllocSize(alloc) >= (usize)data))
	{
		return 0;
	}

	return 1;
}

// Finds or creates a node of the required size
static AllocNode_t* GetFreeNode(usize size)
{
	AllocNode_t* alloc = s_allocations.Find(FindFreeNode, reinterpret_cast<void*>(size));
	if (!alloc)
	{
		alloc = MakeNewNode(size);
		return alloc; // Can skip next part because the node will be the exact size
	}

	// Free the rest of the block
	if (alloc->data.size - size > sizeof(AllocNode_t))
	{
		AllocNode_t* next = reinterpret_cast<AllocNode_t*>(reinterpret_cast<u8*>(alloc + 1) + alloc->data.size);
		next->data.size = alloc->data.size - size;
		next->data.isFree = true;
		alloc->data.size = size;
		// Keep sorted
		s_allocations.InsertAfter(alloc, next);
	}

	return alloc;
}

static AllocNode_t* GetNode(void* block)
{
	return static_cast<AllocNode_t*>(block) - 1;
}

static bool AreContiguous(AllocNode_t* a, AllocNode_t* b)
{
	return a->data.systemAllocation == b->data.systemAllocation;
}

// Merges any adjacent (in the allocation list), contiguous (in the same system node) allocations that aren't in use
// TODO: add more advanced ways of de-fragmenting
static void CoalesceAllocations()
{
	if (s_allocations.Size() > 1)
	{
		for (AllocNode_t* alloc = s_allocations.GetHead(); !alloc->GetNext()->IsTail(); alloc = alloc->GetNext())
		{
			AllocNode_t* next = alloc->GetNext();
			if (next->data.isFree && AreContiguous(alloc, next))
			{
				alloc->data.size += next->data.size;
				s_allocations.Remove(next);
			}
		}
	}
}

BASEAPI void* Base_Alloc(usize size)
{
	AllocNode_t* alloc = GetFreeNode(sizeof(AllocNode_t) + size);
	if (alloc)
	{
		alloc->data.isFree = false;
		void* block = static_cast<void*>(alloc + 1);
		Base_MemSet(block, 0, size);
		return block;
	}

	return nullptr;
}

// Resize an allocation. If the new size is smaller, possibly make a new free node of the regained space. If it's larger, attempt
// to take from the next contiguous node. Otherwise, make a new allocation and copy data over.
BASEAPI void* Base_Realloc(void* block, usize newSize)
{
	if (!block)
	{
		return nullptr;
	}

	if (newSize == 0)
	{
		Base_Free(block);
		return nullptr;
	}

	AllocNode_t* node = GetNode(block);

	// Shrink this node
	if (EffectiveSize(node) > newSize)
	{
		usize freeSize = EffectiveSize(node) - newSize;
		node->data.size = newSize + sizeof(AllocNode_t);

		// If it's the tail and there's enough space, make a new node after
		if (node->IsTail() && freeSize > sizeof(AllocNode_t))
		{
			AllocInfo_t newNode;
			newNode.isFree = true;
			newNode.size = freeSize;
			newNode.systemAllocation = node->data.systemAllocation;
			s_allocations.InsertAfter(node, reinterpret_cast<AllocNode_t*>((static_cast<u8*>(block) + node->data.size)));
		}
		// Otherwise, move up the next node and increase its size
		else if (!node->IsTail() && node->GetNext()->data.isFree)
		{
			u8* newNext = static_cast<u8*>(block) + node->data.size;
			Base_MemCpy(newNext, node->GetNext(), sizeof(AllocNode_t));
			s_allocations.Remove(node->GetNext());
			s_allocations.InsertAfter(node, reinterpret_cast<AllocNode_t*>(newNext));
			node->GetNext()->data.size += freeSize;
		}

		return block;
	}
	// Either take from the next node, or just free this one and get another one that fits
	else if (!node->IsTail())
	{
		AllocInfo_t* neighbor = &node->GetNext()->data;
		// the node isn't part of the size change
		usize extraSize = newSize - node->data.size - sizeof(AllocNode_t);
		node->data.size = newSize;

		if (AreContiguous(node, node->GetNext()) && neighbor->isFree && EffectiveAllocSize(neighbor) > extraSize)
		{

			u8* newNext = static_cast<u8*>(block) + node->data.size;
			Base_MemCpy(newNext, node->GetNext(), sizeof(AllocNode_t));
			s_allocations.Remove(node->GetNext());
			s_allocations.InsertAfter(node, reinterpret_cast<AllocNode_t*>(newNext));
			node->GetNext()->data.size -= extraSize;

			// Zero the new space added
			Base_MemSet(static_cast<u8*>(block) + node->data.size - extraSize, 0, extraSize);

			return block;
		}
	}

	// If execution reaches here, free this node and get a new one, copying over the contents
	void* newBlock = Base_Alloc(newSize);
	if (!newBlock)
	{
		return nullptr;
	}

	Base_MemCpy(newBlock, block, EffectiveSize(node));
	Base_Free(block);
	return newBlock;
}

BASEAPI void Base_Free(void* block)
{
	if (block)
	{
		GetNode(block)->data.isFree = true;
		CoalesceAllocations();
	}
}
