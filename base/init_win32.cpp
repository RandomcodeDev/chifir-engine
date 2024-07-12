#include "base/platform.h"
#include "platform_win32.h"

BASEAPI void Plat_Init()
{
	if (!Base_InitLoader())
	{
		Base_Quit(NtCurrentTeb()->LastStatusValue, "Failed to initialize dynamic loader");
	}

#ifdef CH_XBOX360
	// 64K pages are the default
	g_systemInfo.PageSize = 0x16000;
#else
	NTSTATUS status = NtQuerySystemInformation(SystemBasicInformation, &g_systemInfo, sizeof(SYSTEM_BASIC_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		Base_Quit(status, "Failed to get basic system information");
	}
#endif
}

BASEAPI void Plat_Shutdown()
{
}
