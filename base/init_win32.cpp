#include "base/platform.h"
#include "platform_win32.h"

BASEAPI void Plat_Init()
{
	if (!Base_InitLoader())
	{
		Base_Quit((u32)NtCurrentTeb()->LastStatusValue, "Failed to initialize dynamic loader");
	}

	NTSTATUS status = NtQuerySystemInformation(SystemBasicInformation, &g_systemInfo, sizeof(SYSTEM_BASIC_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		Base_Quit((u32)status, "Failed to get basic system information");
	}
}

BASEAPI void Plat_Shutdown()
{
}
