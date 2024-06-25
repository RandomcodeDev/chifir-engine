#include "base/compiler.h"
#include "base/types.h"
#include "misc.h"

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
