#include "base/platform.h"
#include "platform_win32.h"

BASEAPI void Plat_Init()
{
	if (!Base_InitLoader())
	{
		Base_Quit((u32)NtCurrentTeb()->LastStatusValue, "Failed to initialize dynamic loader");
	}

	if (!Base_InitMemory())
	{
		Base_Quit((u32)NtCurrentTeb()->LastStatusValue, "Failed to initialize memory allocation");
	}
}

BASEAPI void Plat_Shutdown()
{
	Base_ReleaseMemory();
}
