// This is a basic general allocator that can serve as a basis for more specialized things like bump allocators. It's implemented
// on top of Base_GetSystemMemory, and uses a free list with the nodes as headers to chunks of memory. It still needs to be tested
// more thoroughly, and is missing error logging, statistics, and thread safety. It also can't handle individual allocations
// larger than 64MB. Additionally, fragmentation is yet to be handled.

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
	u8 alignment;
	LinkedNode_t<SystemAllocation_t>* systemAllocation;
	u64 signature; // Must equal ALLOC_SIGNATURE
};

typedef LinkedNode_t<AllocInfo_t> AllocNode_t;

// List of unused blocks. Nodes are stored at the start of each chunk of memory in the allocator's control, and their size is
// included in AllocInfo_t's size member.
// TODO: once threading has been implemented, this MUST be made thread safe.
static CLinkedList<AllocInfo_t> s_free;

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
static const ssize MINIMUM_ALIGNMENT = ALIGNOF(AllocNode_t);

// Maximum alignment of allocations, mainly to set an upper bound on how far back to search for a node in an allocation
static const ssize MAXIMUM_ALIGNMENT = 64;

// This finds a system node with the requested size, or allocates a new one of a fixed size, and then makes a new AllocNode_t
static AllocNode_t* MakeNewNode(ssize size)
{
	LinkedNode_t<SystemAllocation_t>* node = g_memInfo.allocations.Find(FindSystemNode, reinterpret_cast<void*>(size));
	if (!node)
	{
		ASSERT_MSG_SAFE(Base_GetSystemMemory(SYSTEM_ALLOC_SIZE) != false, "system memory allocator exhausted or not initialized");
		node = g_memInfo.allocations.GetTail();
	}

	AllocNode_t* alloc = reinterpret_cast<AllocNode_t*>((static_cast<u8*>(node->data.memory) + node->data.used));
	alloc->data.size = size;
	alloc->data.systemAllocation = node;
	alloc->data.signature = ALLOC_SIGNATURE;

	node->data.used += alloc->data.size;
	g_memInfo.used += alloc->data.size;

	return alloc;
}

static ssize EffectiveAllocSize(AllocInfo_t* info)
{
	return info->size - SIZEOF(AllocNode_t);
}

static ssize EffectiveSize(AllocNode_t* node)
{
	return EffectiveAllocSize(&node->data);
}

static ssize VisibleSize(AllocNode_t* node)
{
	uptr nodeAddr = reinterpret_cast<uptr>(node);
	return EffectiveSize(node) - (ALIGN(nodeAddr, node->data.alignment) - nodeAddr);
}

static void* VisibleStart(AllocNode_t* node)
{
	return reinterpret_cast<void*>(reinterpret_cast<uptr>(node) + node->data.size - VisibleSize(node));
}

static s32 FindFreeNode(AllocInfo_t* alloc, void* data)
{
	if (EffectiveAllocSize(alloc) >= (ssize)data)
	{
		return 0;
	}

	return 1;
}

// Finds or creates a node of the required size
static AllocNode_t* GetFreeNode(ssize size)
{
	AllocNode_t* alloc = s_free.Find(FindFreeNode, reinterpret_cast<void*>(size));
	if (!alloc)
	{
		return nullptr;
	}

	// Free the rest of the block
	if (alloc->data.size - size > SIZEOF(AllocNode_t))
	{
		AllocNode_t* next = reinterpret_cast<AllocNode_t*>(reinterpret_cast<uptr>(alloc) + size);
		next->data.size = alloc->data.size - size;
		alloc->data.size = size;
		alloc->data.signature = ALLOC_SIGNATURE;
		// Keep sorted
		s_free.InsertAfter(alloc, next);
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
		ASSERT_MSG_SAFE(
			reinterpret_cast<sptr>(block) - reinterpret_cast<sptr>(sigAddr) < MAXIMUM_ALIGNMENT + SIZEOF(AllocNode_t),
			"invalid allocation: couldn't find signature before the given address");
		sigAddr--;
	}

#ifdef __clang__
	// data should be at a constant offset in AllocNode_t
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
	AllocNode_t* node = reinterpret_cast<AllocNode_t*>(sigAddr - offsetof(AllocInfo_t, signature) - offsetof(AllocNode_t, data));
#ifdef __clang__
#pragma clang diagnostic pop
#endif
	return node;
}

static bool SameSystemNode(AllocNode_t* a, AllocNode_t* b)
{
	return a->data.systemAllocation == b->data.systemAllocation;
}

static bool Contiguous(AllocNode_t* a, AllocNode_t* b)
{
	return SameSystemNode(a, b) && reinterpret_cast<sptr>(a) + a->data.size == reinterpret_cast<sptr>(b);
}

// Merges any adjacent (in the allocation list), contiguous (in the same system node) allocations that aren't in use
// TODO: add more advanced ways of de-fragmenting
static void CoalesceAllocations()
{
	if (s_free.Size() > 1)
	{
		for (AllocNode_t* alloc = s_free.GetHead(); !alloc->IsTail() && !alloc->GetNext()->IsTail(); alloc = alloc->GetNext())
		{
			AllocNode_t* next = alloc->GetNext();
			if (Contiguous(alloc, next))
			{
				alloc->data.size += next->data.size;
				s_free.Remove(next);
			}
		}
	}
}

static ssize FixAlignment(ssize alignment)
{
	return Min(Max(alignment, MINIMUM_ALIGNMENT), MAXIMUM_ALIGNMENT);
}

BASEAPI ALLOCATOR void* Base_Alloc(ssize size, ssize alignment)
{
	ssize realAlignment = FixAlignment(alignment);
	ssize realSize = ALIGN(SIZEOF(AllocNode_t) + size, realAlignment);

	ASSERT_MSG_SAFE(realAlignment % 2 == 0, "alignment must be a power of 2");
	ASSERT_MSG_SAFE(realSize <= SYSTEM_ALLOC_SIZE - SIZEOF(AllocNode_t), "size must be less than or equal to 67108832");

	AllocNode_t* alloc = GetFreeNode(realSize);
	if (alloc)
	{
		s_free.Remove(alloc);
	}
	else
	{
		alloc = MakeNewNode(realSize);
	}

	if (alloc)
	{
		alloc->data.alignment = static_cast<u8>(realAlignment);
		void* block = VisibleStart(alloc);
		Base_MemSet(block, 0, size);
		g_memInfo.totalAllocated += size;
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
		node->data.size = newSize + SIZEOF(AllocNode_t);

		// If it's the tail and there's enough space, make a new node after
		if (node->IsTail() && freeSize > SIZEOF(AllocNode_t))
		{
			AllocInfo_t newNode;
			newNode.size = freeSize;
			newNode.systemAllocation = node->data.systemAllocation;
			newNode.signature = ALLOC_SIGNATURE;
			s_free.InsertAfter(node, reinterpret_cast<AllocNode_t*>((static_cast<u8*>(block) + node->data.size)));
			Base_MemCopy(&node->GetNext()->data, &newNode, SIZEOF(AllocInfo_t));
		}
		// Otherwise, move up the next node and increase its size
		else if (!node->IsTail())
		{
			u8* newNext = static_cast<u8*>(block) + node->data.size;
			Base_MemCopy(newNext, node->GetNext(), SIZEOF(AllocNode_t));
			s_free.Remove(node->GetNext());
			s_free.InsertAfter(node, reinterpret_cast<AllocNode_t*>(newNext));
			node->GetNext()->data.size += freeSize;
		}

		return block;
	}
	// Either take from the next node, or just free this one and get another one that fits
	else if (!node->IsTail())
	{
		AllocInfo_t* neighbor = &node->GetNext()->data;
		// the node isn't part of the size change
		ssize extraSize = newSize - node->data.size - SIZEOF(AllocNode_t);

		if (Contiguous(node, node->GetNext()) && EffectiveAllocSize(neighbor) > extraSize)
		{
			node->data.size = newSize + SIZEOF(AllocNode_t);

			u8* newNext = static_cast<u8*>(block) + node->data.size;
			Base_MemCopy(newNext, node->GetNext(), SIZEOF(AllocNode_t));
			s_free.Remove(node->GetNext());
			s_free.InsertAfter(node, reinterpret_cast<AllocNode_t*>(newNext));
			node->GetNext()->data.size -= extraSize;

			// Zero the new space added
			Base_MemSet(static_cast<u8*>(block) + node->data.size - extraSize, 0, extraSize);

			g_memInfo.totalAllocated += extraSize;
			return block;
		}
	}

	// If execution reaches here, free this node and get a new one, copying over the contents
	void* newBlock = Base_Alloc(newSize, node->data.alignment);
	if (!newBlock)
	{
		return nullptr;
	}

	Base_MemCopy(newBlock, block, VisibleSize(node));
	Base_Free(block);
	return newBlock;
}

static void InsertFreedNode(AllocNode_t* node)
{
	if (s_free.IsEmpty())
	{
		s_free.Prepend(node);
		return;
	}

	// Find the appropriate position to insert the node to keep the list sorted by address
	AllocNode_t* current = s_free.GetHead();
	while (!current->IsTail() && (current->data.systemAllocation != node->data.systemAllocation ||
								  reinterpret_cast<sptr>(current) < reinterpret_cast<sptr>(node)))
	{
		current = current->GetNext();
	}

	if (current->IsHead())
	{
		s_free.Prepend(node);
	}
	else
	{
		s_free.InsertBefore(current, node);
	}
}

BASEAPI void Base_Free(void* block)
{
	// TODO: check if block is within an allocation
	if (block)
	{
		AllocNode_t* node = FindNode(block);
		InsertFreedNode(node);
		g_memInfo.totalFreed += EffectiveSize(node);
		CoalesceAllocations();
	}
}
