// This is a basic general allocator that can serve as a basis for more specialized things like bump allocators. It's implemented
// on top of Base_GetSystemMemory, and uses a linked list of allocations, with the nodes as headers to chunks of memory. It still
// needs to be tested more thoroughly, and is missing logging for errors and thread safety. It also can't handle individual
// allocations larger than 64MB.

#include "base.h"
#include "base/base.h"
#include "base/dll.h"
#include "base/types.h"

MemoryInfo_t g_memInfo;

// Signature so header can be found in aligned blocks (also is 'CHFALLOC' in ASCII on little endian)
static const u64 ALLOC_SIGNATURE = 0x434F4C4C41464843;

struct AllocInfo_t
{
	ssize size; // Includes the size of the AllocNode_t header
	bool isFree;
	u8 alignment;
	LinkedNode_t<SystemAllocation_t>* systemAllocation;
	u64 signature; // Must equal ALLOC_SIGNATURE
};

typedef LinkedNode_t<AllocInfo_t> AllocNode_t;

// List of allocations. Nodes are stored at the start of each chunk of memory in the allocator's control, and their size is
// included in AllocInfo_t's size member.
// TODO: once threading has been implemented, this MUST be made thread safe.
static CLinkedList<AllocInfo_t> s_allocations;

static s32 FindSystemNode(SystemAllocation_t* alloc, void* data)
{
	if (alloc->size - alloc->used >= (ssize)data)
	{
		return 0;
	}

	return 1;
}

// Size of system nodes which are directly allocated with NtAllocateVirtualMemory/mmap
static const ssize SYSTEM_ALLOC_SIZE = 64 * 1024 * 1024;

// Minimum alignment of allocations
static const ssize MINIMUM_ALIGNMENT = alignof(AllocNode_t);

// Maximum alignment of allocations, mainly to set an upper bound on how far back to search for a node in an allocation
static const ssize MAXIMUM_ALIGNMENT = 64;

// This finds a system node with the requested size, or allocates a new one of a fixed size
static AllocNode_t* MakeNewNode(ssize size)
{
	LinkedNode_t<SystemAllocation_t>* node = g_memInfo.allocations.Find(FindSystemNode, reinterpret_cast<void*>(size));
	if (!node)
	{
		ASSERT_MSG(Base_GetSystemMemory(SYSTEM_ALLOC_SIZE), "system memory allocator exhausted");
		node = g_memInfo.allocations.GetTail();
	}

	AllocNode_t* alloc = reinterpret_cast<AllocNode_t*>((static_cast<u8*>(node->data.memory) + node->data.used));
	alloc->data.size = size;
	alloc->data.isFree = false;
	alloc->data.systemAllocation = node;
	alloc->data.signature = ALLOC_SIGNATURE;

	node->data.used += alloc->data.size;

	s_allocations.Append(alloc);

	return alloc;
}

static ssize EffectiveAllocSize(AllocInfo_t* info)
{
	return info->size - sizeof(AllocNode_t);
}

static ssize EffectiveSize(AllocNode_t* node)
{
	return EffectiveAllocSize(&node->data);
}

static s32 FindFreeNode(AllocInfo_t* alloc, void* data)
{
	if (alloc->isFree && (EffectiveAllocSize(alloc) >= (ssize)data))
	{
		return 0;
	}

	return 1;
}

// Finds or creates a node of the required size
static AllocNode_t* GetFreeNode(ssize size)
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
		alloc->data.signature = ALLOC_SIGNATURE;
		// Keep sorted
		s_allocations.InsertAfter(alloc, next);
	}

	return alloc;
}

// Finds the header for a block by its signature
static AllocNode_t* FindNode(void* block)
{
	u8* sigAddr = static_cast<u8*>(block);
	// Scan for the signature, then find the address of the AllocNode_t signature it belongs to
	while (*reinterpret_cast<u64*>(sigAddr) != ALLOC_SIGNATURE)
	{
		// Ensure that this doesn't go so far back that a different allocation is found
		ASSERT_MSG(
			reinterpret_cast<sptr>(block) - reinterpret_cast<sptr>(sigAddr) < MAXIMUM_ALIGNMENT + sizeof(AllocNode_t),
			"invalid allocation: couldn't find signature less than %zu bytes before the given address",
			reinterpret_cast<sptr>(block) - reinterpret_cast<sptr>(sigAddr));
		sigAddr--;
	}

	AllocNode_t* node = reinterpret_cast<AllocNode_t*>(sigAddr - offsetof(AllocInfo_t, signature) - offsetof(AllocNode_t, data));
	return node;
}

static bool SameSystemNode(AllocNode_t* a, AllocNode_t* b)
{
	return a->data.systemAllocation == b->data.systemAllocation;
}

// Merges any adjacent (in the allocation list), contiguous (in the same system node) allocations that aren't in use
// TODO: add more advanced ways of de-fragmenting
static void CoalesceAllocations()
{
	if (s_allocations.Size() > 1)
	{
		for (AllocNode_t* alloc = s_allocations.GetHead(); !alloc->IsTail() && !alloc->GetNext()->IsTail(); alloc = alloc->GetNext())
		{
			AllocNode_t* next = alloc->GetNext();
			if (next->data.isFree && SameSystemNode(alloc, next))
			{
				alloc->data.size += next->data.size;
				s_allocations.Remove(next);
			}
		}
	}
}

static ssize FixAlignment(ssize alignment)
{
	return Min(Max(alignment, MINIMUM_ALIGNMENT), MAXIMUM_ALIGNMENT);
}

BASEAPI void* Base_Alloc(ssize size, ssize alignment)
{
	ssize realAlignment = FixAlignment(alignment);

	ASSERT_MSG(realAlignment <= MAXIMUM_ALIGNMENT, "alignment must be less than or equal to %zu", MAXIMUM_ALIGNMENT);
	ASSERT_MSG(realAlignment % 2 == 0, "alignment must be a power of 2");

	AllocNode_t* alloc = GetFreeNode(ALIGN(sizeof(AllocNode_t) + size, realAlignment));
	if (alloc)
	{
		alloc->data.isFree = false;
		// Round up the address of the first byte after the AllocNode_t to be aligned
		void* block = reinterpret_cast<void*>(ALIGN(reinterpret_cast<uptr>(alloc + 1), realAlignment));
		Base_MemSet(block, 0, size);
		return block;
	}

	return nullptr;
}

// Resize an allocation. If the new size is smaller, possibly make a new free node of the regained space. If it's larger, attempt
// to take from the next contiguous node. Otherwise, make a new allocation and copy data over.
BASEAPI void* Base_Realloc(void* block, ssize newSize)
{
	if (!block)
	{
		return Base_Alloc(newSize);
	}

	if (newSize == 0)
	{
		Base_Free(block);
		return nullptr;
	}

	AllocNode_t* node = FindNode(block);

	// Shrink this node
	if (EffectiveSize(node) > newSize)
	{
		ssize freeSize = EffectiveSize(node) - newSize;
		node->data.size = newSize + sizeof(AllocNode_t);

		// If it's the tail and there's enough space, make a new node after
		if (node->IsTail() && freeSize > sizeof(AllocNode_t))
		{
			AllocInfo_t newNode;
			newNode.isFree = true;
			newNode.size = freeSize;
			newNode.systemAllocation = node->data.systemAllocation;
			newNode.signature = ALLOC_SIGNATURE;
			s_allocations.InsertAfter(node, reinterpret_cast<AllocNode_t*>((static_cast<u8*>(block) + node->data.size)));
			Base_MemCopy(&node->GetNext()->data, &newNode, sizeof(AllocInfo_t));
		}
		// Otherwise, move up the next node and increase its size
		else if (!node->IsTail() && node->GetNext()->data.isFree)
		{
			u8* newNext = static_cast<u8*>(block) + node->data.size;
			Base_MemCopy(newNext, node->GetNext(), sizeof(AllocNode_t));
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
		ssize extraSize = newSize - node->data.size - sizeof(AllocNode_t);
		node->data.size = newSize;

		if (SameSystemNode(node, node->GetNext()) && neighbor->isFree && EffectiveAllocSize(neighbor) > extraSize)
		{

			u8* newNext = static_cast<u8*>(block) + node->data.size;
			Base_MemCopy(newNext, node->GetNext(), sizeof(AllocNode_t));
			s_allocations.Remove(node->GetNext());
			s_allocations.InsertAfter(node, reinterpret_cast<AllocNode_t*>(newNext));
			node->GetNext()->data.size -= extraSize;

			// Zero the new space added
			Base_MemSet(static_cast<u8*>(block) + node->data.size - extraSize, 0, extraSize);

			return block;
		}
	}

	// If execution reaches here, free this node and get a new one, copying over the contents
	void* newBlock = Base_Alloc(newSize, node->data.alignment);
	if (!newBlock)
	{
		return nullptr;
	}

	Base_MemCopy(newBlock, block, EffectiveSize(node));
	Base_Free(block);
	return newBlock;
}

BASEAPI void Base_Free(void* block)
{
	if (block)
	{
		FindNode(block)->data.isFree = true;
		CoalesceAllocations();
	}
}
