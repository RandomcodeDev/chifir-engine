#include "base/platform.h"

extern "C" BOOL __stdcall _DllMainCRTStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
    (void)hDllHandle;
    (void)lpreserved;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		__security_init_cookie();
	}

    return TRUE;
}
