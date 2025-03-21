/// Internal base definitions

#pragma once

#include "base/compiler.h"
#include "base/linkedlist.h"
#include "base/types.h"

/// CPU data
struct BaseCpuData_t
{
#ifdef CH_X86
	char brand[13];   /// GenuineIntel/AuthenticAMD
	char name[49];    /// CPU model name
	u32 haveName : 1; /// CPUID with EAX = 0x80000000 returned 0x80000004 or higher
#endif
	u32 haveSimd128 : 1;        /// x86 SSE / ARM NEON / Xbox 360 VMX128
	u32 haveIntSimd128 : 1;     /// x86 SSE2 / ARM NEON / Xbox 360 VMX128
	u32 haveSimd128Compare : 1; /// x86 SSE4.2 / ARM NEON
	u32 haveSimd256 : 1;        /// x86 AVX / ARM NEON
};

/// Whether Base_Init has succeeded
extern bool g_baseInitialized;

/// Whether Plat_Init has succeeded
extern bool g_platInitialized;

/// Whether the allocator is usabble
extern bool g_allocUsable;

/// Initialized in Base_Init
extern BaseCpuData_t g_cpuData;

/// OS allocation
struct SystemAllocation_t
{
	void* memory;
	ssize used;
	ssize size;
};

/// Memory info
struct MemoryInfo_t
{
	CIntrusiveLinkedList<SystemAllocation_t> allocations;
	ssize used;
	ssize size;
	ssize totalAllocated;
	ssize totalFreed;
};

extern MemoryInfo_t g_memInfo;

/// Allocate a chunk of memory from the OS for the allocator
extern bool Base_GetSystemMemory(ssize size);

/// Release a chunk of memory
extern void Base_ReleaseSystemMemory(IntrusiveLinkedNode_t<SystemAllocation_t> alloc);

/// Release all system memory, you shouldn't call this unless you're done with the allocator and any memory it owns
extern void Base_ReleaseAllMemory();
