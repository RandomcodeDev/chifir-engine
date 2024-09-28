#include "platform_win32.h"
#include "base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/platform.h"
#include "base/types.h"

DECLARE_AVAILABLE(DbgPrint);
DECLARE_AVAILABLE(NtAllocateVirtualMemory);
#ifdef CH_XBOX360
extern "C" DLLIMPORT void __stdcall XamTerminateTitle();
#else
DECLARE_AVAILABLE(NtTerminateProcess);
#endif

SYSTEM_BASIC_INFORMATION g_systemInfo;
SYSTEM_PERFORMANCE_INFORMATION g_systemPerfInfo;

static char* s_systemDescription;
static char* s_hardwareDescription;

BASEAPI void Plat_Init()
{
	// This is to allow the loader to use memory allocation for library names
#ifdef CH_XBOX360
	// 64K pages are the default
	g_systemInfo.PageSize = 0x10000;
#else
	// Every other supported architecture uses 4K pages
	g_systemInfo.PageSize = 0x1000;
#endif
	g_allocUsable = true;

	if (!Base_InitLoader())
	{
		Base_AbortSafe(LastNtStatus(), "Failed to initialize dynamic loader");
	}

#ifndef CH_XBOX360
	// Properly determine the page size just in case, and get other info
	NTSTATUS status = NtQuerySystemInformation(SystemBasicInformation, &g_systemInfo, SIZEOF(SYSTEM_BASIC_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		Base_Abort(status, "Failed to get basic system information: NTSTATUS 0x%08X", status);
	}

	// Need this for RAM available
	status = NtQuerySystemInformation(
		SystemPerformanceInformation, &g_systemPerfInfo, SIZEOF(SYSTEM_PERFORMANCE_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		Base_Abort(status, "Failed to get system performance information: NTSTATUS 0x%08X", status);
	}
#endif

	(void)Plat_GetSystemDescription();
	(void)Plat_GetHardwareDescription();

	g_platInitialized = true;
}

BASEAPI void Plat_Shutdown()
{
	if (s_systemDescription)
	{
		Base_Free(s_systemDescription);
	}
	if (s_hardwareDescription)
	{
		Base_Free(s_hardwareDescription);
	}
}

BASEAPI cstr Plat_GetSystemDescription()
{
	if (!s_systemDescription)
	{
#ifdef CH_XBOX360
		// TODO: figure this out, probably easy
		s_systemDescription = Base_StrClone("");
#else
		s_systemDescription = Base_StrFormat(
			"Windows %u.%u.%u (reported as %u.%u.%u)", USER_SHARED_DATA->NtMajorVersion,
			USER_SHARED_DATA->NtMinorVersion, USER_SHARED_DATA->NtBuildNumber, NtCurrentPeb()->OSMajorVersion,
			NtCurrentPeb()->OSMinorVersion, NtCurrentPeb()->OSBuildNumber);
#endif
	}

	return s_systemDescription;
}

BASEAPI cstr Plat_GetHardwareDescription()
{
	if (!s_hardwareDescription)
	{
#ifdef CH_XBOX360
		// TODO: figure this out, should be easy
		s_hardwareDescription = Base_StrClone("");
#else
#ifdef CH_X86
		s64 freeMemory = g_systemPerfInfo.AvailablePages * static_cast<s64>(g_systemInfo.PageSize);
		s64 physicalMemory = g_systemInfo.NumberOfPhysicalPages * static_cast<s64>(g_systemInfo.PageSize);

		// TODO: do stuff with the funny numbers instead of relying on the name being given directly
		s_hardwareDescription = Base_StrFormat(
			"%s %s with %s of RAM (%s free)", g_cpuData.brand, g_cpuData.haveName ? g_cpuData.name : "Unknown",
			Base_FormatSize(physicalMemory).Data(), Base_FormatSize(freeMemory).Data());
#else
		s_hardwareDescription = Base_StrClone("");
#endif
#endif
	}

	return s_hardwareDescription;
}

BASEAPI NORETURN void Base_AbortSafe(s32 code, cstr msg)
{
	if (code == 1 && LastNtStatus() != 0)
	{
		code = LastNtStatus();
	}
	else if (code == 1 && LastNtError() != 0)
	{
		code = LastNtError();
	}
	else if (code == 1)
	{
		code = STATUS_FATAL_APP_EXIT;
	}

	if (DbgPrint_Available())
	{
		DbgPrint("%s\n", msg);
	}

	// TODO: use NtRaiseHardError

	BREAKPOINT();
#ifdef CH_XBOX360
	XamTerminateTitle();
#else
	if (NtTerminateProcess_Available())
	{
		NtTerminateProcess(NtCurrentProcess(), static_cast<NTSTATUS>(code));
	}
#endif

	ASSUME(0);
}

bool Base_GetSystemMemory(ssize size)
{
	// Linked list nodes, can contain any size of allocation, but there's a limit to the number of OS allocations (this should be
	// changed in the future, this is based on a vacuum currently)
	static LinkedNode_t<SystemAllocation_t> memoryNodes[64];

	if (!NtAllocateVirtualMemory_Available())
	{
		return false;
	}

	size = ALIGN(size, g_systemInfo.PageSize);

	ASSERT_MSG(
		g_memInfo.allocations.Size() < ARRAY_SIZE(memoryNodes),
		"OS allocation nodes exhausted, increase the size of the memory nodes array");

	LinkedNode_t<SystemAllocation_t>* node = &memoryNodes[g_memInfo.allocations.Size()];
	node->data.size = size;

	NTSTATUS status =
		NtAllocateVirtualMemory(NtCurrentProcess(), &node->data.memory, 0, (PSIZE_T)&node->data.size, MEM_COMMIT, PAGE_READWRITE);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		return false;
	}

	g_memInfo.size += size;
	g_memInfo.allocations.Append(node);

	return true;
}

void Base_ReleaseSystemMemory(LinkedNode_t<SystemAllocation_t>* allocation)
{
	ssize size = 0;
	NtFreeVirtualMemory(NtCurrentProcess(), &allocation->data.memory, (PSIZE_T)&size, MEM_RELEASE);
	g_memInfo.allocations.Remove(allocation);
}

void Base_ReleaseAllMemory()
{
	for (LinkedNode_t<SystemAllocation_t>* cur = g_memInfo.allocations.GetHead(); cur; cur = cur->GetNext())
	{
		Base_ReleaseSystemMemory(cur);
	}
}
