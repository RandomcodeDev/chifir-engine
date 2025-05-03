/// \file Internal base definitions
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/async.h"
#include "base/compiler.h"
#include "base/linkedlist.h"
#include "base/types.h"

/// CPU data
struct BaseCpuData
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
extern BaseCpuData g_cpuData;

/// OS allocation
struct SystemAllocation
{
	void* memory;
	ssize used;
	ssize size;
};

/// Memory info
struct MemoryInfo
{
	CIntrusiveLinkedList<SystemAllocation> allocations;
	ssize used;
	ssize size;
	ssize totalAllocated;
	ssize totalFreed;
};

extern MemoryInfo g_memInfo;

/// Initialize the memory allocator
extern void Base_InitAllocator();

/// Shut down the memory allocator
extern void Base_ShutdownAllocator();

/// Allocate a chunk of memory from the OS for the allocator
extern bool Base_GetSystemMemory(ssize size);

/// Release a chunk of memory
extern void Base_ReleaseSystemMemory(IntrusiveLinkedNode<SystemAllocation> alloc);

/// Release all system memory, you shouldn't call this unless you're done with the allocator and any memory it owns
extern void Base_ReleaseAllMemory();

/// Initialize the logging system
extern void Log_Init();

/// Shut down the logging system
extern void Log_Shutdown();
