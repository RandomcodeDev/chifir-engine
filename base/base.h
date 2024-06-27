// Internal base definitions

#pragma once

#include "base/compiler.h"
#include "base/types.h"

// CPU data, comments are in x86 terms until I actually understand other archs
struct BaseCpuData_t
{
#ifdef KR_X86
	char brand[13];   // GenuineIntel/AuthenticAMD
	char name[65];    // CPU model name
	u32 haveName : 1; // EAX = 0x80000000 returned 0x80000004 or higher
#endif
	u32 haveSimd128 : 1;    // SSE
	u32 haveIntSimd128 : 1; // SSE2
	u32 haveSimd256 : 1;    // AVX
};

// Whether Base_Init has succeeded
extern bool g_baseInitialized;

// Initialized in Base_Init
extern BaseCpuData_t g_cpuData;

// Memory info
struct MemoryInfo_t
{
	void* memory;
	usize used;
	usize totalSize;
};

extern MemoryInfo_t g_memInfo;

// So individual implementations don't have to handle formatting the message
extern NORETURN void Base_QuitImpl(u32 code, cstr msg);
