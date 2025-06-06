/// \file Memory management
/// \copyright 2025 Randomcode Developers

/// This is a basic general allocator that can serve as a basis for more specialized things like bump allocators. It's implemented
/// on top of Base_GetSystemMemory, and uses a free list with the nodes as headers to chunks of memory. It still needs to be
/// tested more thoroughly, and is missing error logging, statistics, and thread safety. It also can't handle individual
/// allocations larger than 64MB.

#include "base.h"
#include "base/base.h"
#include "base/dll.h"
#include "base/platform.h"
#include "base/types.h"

#ifdef CH_WIN32
#include "platform_win32.h"
#elif defined CH_XBOX
#include "platform_xbox.h"
#elif defined CH_UNIX
#include "platform_unix.h"
#endif

MemoryInfo g_memInfo;

/// Signature so header can be found in aligned blocks (also is 'CHFALLOC' in ASCII on little endian)
static const u64 ALLOC_SIGNATURE = 0x434F4C4C41464843;

/// TODO: make smaller
struct AllocInfo_t
{
	ssize size; // Includes the size of the AllocNode_t header
	ssize alignment;
	IntrusiveLinkedNode<SystemAllocation>* systemAllocation;
	u64 signature; // Must equal ALLOC_SIGNATURE
};

typedef IntrusiveLinkedNode<AllocInfo_t> AllocNode_t;

#ifdef CH_WIN32
typedef CWindowsMutex AllocMutex_t;
#elif defined CH_XBOX
typedef CXboxMutex AllocMutex_t;
#elif defined CH_UNIX
typedef CUnixMutex AllocMutex_t;
#else
#error "Unknown mutex type"
#endif

/// Memory for s_freeMutex, since it can't be allocated
static u8 s_freeMutexMem[SIZEOF(AllocMutex_t)];

/// Protects the free list, backed by s_freeMutexMem
static IMutex* s_freeMutex;

/// List of unused blocks. Nodes are stored at the start of each chunk of memory in the allocator's control, and their size is
/// included in AllocInfo_t's size member.
static CIntrusiveLinkedList<AllocInfo_t> s_free;

void Base_InitAllocator()
{
	// can't allocate the mutex with new, so some shenanigans have to occur
	s_freeMutex = reinterpret_cast<IMutex*>(s_freeMutexMem);
	new(s_freeMutex) AllocMutex_t;
}

void Base_ShutdownAllocator()
{
	// call the virtual destructor
	s_freeMutex->~IMutex();
}

static s32 FindSystemNode(SystemAllocation* alloc, void* data)
{
	if (alloc->size - alloc->used >= (ssize)data)
	{
		return 0;
	}

	return 1;
}

/// Size of system nodes which are directly allocated with NtAllocateVirtualMemory/mmap
static const ssize SYSTEM_ALLOC_SIZE = 64 * 1024 * 1024;

/// Minimum alignment of allocations
static const ssize MINIMUM_ALIGNMENT = ALIGNOF(AllocNode_t);

/// Maximum alignment of allocations, mainly to set an upper bound on how far back to search for a node in an allocation
static const ssize MAXIMUM_ALIGNMENT = 64;

/// This finds a system node with the requested size, or allocates a new one of a fixed size, and then makes a new AllocNode_t
static AllocNode_t* MakeNewNode(ssize size)
{
	IntrusiveLinkedNode<SystemAllocation>* node = g_memInfo.allocations.Find(FindSystemNode, reinterpret_cast<void*>(size));
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
	return static_cast<ssize>(EffectiveSize(node) - (AlignUp(nodeAddr, node->data.alignment) - nodeAddr));
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

/// Finds or creates a node of the required size
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
		next->data.systemAllocation = alloc->data.systemAllocation;
		next->data.size = alloc->data.size - size;
		next->data.signature = ALLOC_SIGNATURE;

		alloc->data.size = size;
		alloc->data.signature = ALLOC_SIGNATURE;

		// Keep sorted
		s_freeMutex->Lock();
		s_free.InsertAfter(alloc, next);
		s_freeMutex->Unlock();
	}

	return alloc;
}

/// Finds the header for a block by its signature
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
	// data is at a constant offset in AllocNode_t, but it's a template so clang is rightfully paranoid
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
	AllocNode_t* node = reinterpret_cast<AllocNode_t*>(sigAddr - OFFSETOF(AllocInfo_t, signature) - OFFSETOF(AllocNode_t, data));
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

/// Merges any free allocations that are on the same system node and are back to back
/// TODO: add more advanced ways of de-fragmenting
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
				s_freeMutex->Lock();
				s_free.Remove(next);
				s_freeMutex->Unlock();
				Base_MemSet(&next->data, 0, SIZEOF(AllocInfo_t));
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
	ssize realSize = AlignUp(SIZEOF(AllocNode_t) + size, realAlignment);

	ASSERT_MSG_SAFE(realAlignment % 2 == 0, "alignment must be a power of 2");
	ASSERT_MSG_SAFE(realSize <= SYSTEM_ALLOC_SIZE - SIZEOF(AllocNode_t), "size must be less than or equal to 67108832");

	AllocNode_t* alloc = GetFreeNode(realSize);
	if (alloc)
	{
		s_freeMutex->Lock();
		s_free.Remove(alloc);
		s_freeMutex->Unlock();
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

/// Resize an allocation. Currently just makes a new allocation and copies data over.
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

BASEAPI void Base_Free(void* block)
{
#ifdef STRICT_ALLOC
	ASSERT_MSG_SAFE(block != nullptr, "Block was nullptr");
#else
	if (!block)
	{
		return;
	}
#endif

	// TODO: check if block is within an allocation
	AllocNode_t* node = FindNode(block);
	g_memInfo.totalFreed += EffectiveSize(node);
	CoalesceAllocations();
}

BASEAPI ssize Base_GetAllocSize(void* block)
{
	if (block)
	{
		AllocNode_t* node = FindNode(block);
		if (node)
		{
			return VisibleSize(node);
		}
	}

	return static_cast<ssize>(-1);
}

BASEAPI ssize Base_GetAllocAlignment(void* block)
{
	if (block)
	{
		AllocNode_t* node = FindNode(block);
		if (node)
		{
			return node->data.alignment;
		}
	}

	return static_cast<ssize>(-1);
}
