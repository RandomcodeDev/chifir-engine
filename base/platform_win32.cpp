#include "platform_win32.h"
#include "base.h"
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
		Base_Abort(status, "Failed to get basic system information: NTSTATUS %#08X", status);
	}
#endif

	g_platInitialized = true;
}

BASEAPI void Plat_Shutdown()
{
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
