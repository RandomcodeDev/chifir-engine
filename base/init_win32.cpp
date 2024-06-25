#include "base/platform.h"
#include "loader_win32.h"

BASEAPI void Plat_Init()
{
	if (!Base_InitLoader())
	{
		Base_Quit((u32)NtCurrentTeb()->LastStatusValue, "Failed to initialize dynamic loader");
	}
}

BASEAPI void Plat_Shutdown()
{
}
