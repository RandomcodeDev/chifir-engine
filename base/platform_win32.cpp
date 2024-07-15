#include "platform_win32.h"
#include "base.h"
#include "base/compiler.h"
#include "base/platform.h"
#include "base/types.h"

extern "C" BASEAPI bool DbgPrint_Available();
extern "C" BASEAPI bool NtAllocateVirtualMemory_Available();
#ifndef CH_XBOX360
extern "C" BASEAPI bool NtTerminateProcess_Available();
#endif

SYSTEM_BASIC_INFORMATION g_systemInfo;

BASEAPI NORETURN void Base_QuitSafe(s32 code, cstr msg)
{
	if (code == 1 && LastNtStatus() != 0)
	{
		code = LastNtStatus();
	}
	else if (code == 2 && LastNtError() != 0)
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
	ExitThread(static_cast<u32>(code));
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
