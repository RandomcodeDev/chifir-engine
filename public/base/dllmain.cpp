#include "base/platform.h"

extern "C" BOOL __stdcall _DllMainCRTStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
    (void)hDllHandle;
    (void)lpreserved;

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		__security_init_cookie();
		RunGlobalConstructors();
		break;
	case DLL_THREAD_ATTACH:
		RunThreadConstructors();
		break;
	case DLL_PROCESS_DETACH:
		RunGlobalDestructors();
		break;
	}

    return TRUE;
}
