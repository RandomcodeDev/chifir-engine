// Internal base definitions

#pragma once

#include "base/compiler.h"
#include "base/linkedlist.h"
#include "base/types.h"

// CPU data, comments are in x86 terms until I actually understand other arches
struct BaseCpuData_t
{
#ifdef CF_X86
	char brand[13];   // GenuineIntel/AuthenticAMD
	char name[65];    // CPU model name
	u32 haveName : 1; // CPUID with EAX = 0x80000000 returned 0x80000004 or higher
#endif
	u32 haveSimd128 : 1;    // SSE
	u32 haveIntSimd128 : 1; // SSE2
	u32 haveSimd256 : 1;    // AVX
};

// Whether Base_Init has succeeded
extern bool g_baseInitialized;

// Initialized in Base_Init
extern BaseCpuData_t g_cpuData;

// OS allocation
struct SystemAllocation_t
{
	void* memory;
	usize used;
	usize size;
};

// Memory info
struct MemoryInfo_t
{
	CLinkedList<SystemAllocation_t> allocations;
	usize used;
	usize size;
};

extern MemoryInfo_t g_memInfo;

// Allocate a chunk of memory from the OS for the allocator
extern bool Base_GetSystemMemory(usize size);

// Release a chunk of memory
extern void Base_ReleaseSystemMemory(LinkedNode_t<SystemAllocation_t>);

// Release all system memory, you shouldn't call this unless you're done with the allocator and any memory it owns
extern void Base_ReleaseAllMemory();

// So individual implementations don't have to handle formatting the message
extern NORETURN void Base_QuitImpl(s32 code, cstr msg);
