#include "base/compiler.h"
#include "base/platform.h"
#include "base/types.h"
#include "base.h"

extern "C" void* __imp_NtTerminateProcess;

NORETURN void Base_QuitImpl(u32 code, cstr msg)
{
	if (code == 1)
	{
		// LastErrorValue/LastStatusValue aren't reliable enough
		code = STATUS_FATAL_APP_EXIT;
	}

	// TODO: use NtRaiseHardError

	__debugbreak();
	if (__imp_NtTerminateProcess)
	{
		NtTerminateProcess(NtCurrentProcess(), (NTSTATUS)code);
	}
	else
	{
		BREAKPOINT();
	}
	
	ASSUME(0);
}

bool Base_InitMemory()
{
	// Memory the program gets for its entire lifetime
	static const usize SIZE = 1 * 1024 * 1024 * 1024;

	g_memInfo.totalSize = SIZE;
	NTSTATUS status = NtAllocateVirtualMemory(NtCurrentProcess(), &g_memInfo.memory, 0, &g_memInfo.totalSize, MEM_RESERVE, PAGE_READWRITE);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		return false;
	}

	return true;
}

void Base_ReleaseMemory()
{
	usize size = 0;
	NtFreeVirtualMemory(NtCurrentProcess(), &g_memInfo.memory, &size, MEM_RELEASE);
}
