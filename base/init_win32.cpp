#include "base.h"
#include "base/platform.h"
#include "platform_win32.h"

BASEAPI void Plat_Init()
{
	// This is to allow the loader to use memory allocation for library names
#ifdef CH_XBOX360
	// 64K pages are the default
	g_systemInfo.PageSize = 0x16000;
#else
	// Every other supported architecture uses 4K pages
	g_systemInfo.PageSize = 0x1000;
#endif
	g_allocUsable = true;

	if (!Base_InitLoader())
	{
		Base_QuitSafe(NtCurrentTeb()->LastStatusValue, "Failed to initialize dynamic loader");
	}

#ifndef CH_XBOX360
	// Properly determine the page size just in case, and get other info
	NTSTATUS status = NtQuerySystemInformation(SystemBasicInformation, &g_systemInfo, sizeof(SYSTEM_BASIC_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		Base_QuitSafe(status, "Failed to get basic system information");
	}
#endif

	g_platInitialized = true;
}

BASEAPI void Plat_Shutdown()
{
}
