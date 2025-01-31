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
#ifdef IN_BASE
		Base_Init();
#endif
		break;
	case DLL_THREAD_ATTACH:
		RunThreadConstructors();
		break;
	case DLL_PROCESS_DETACH:
#ifdef IN_BASE
		Base_Shutdown();
#endif
		RunGlobalDestructors();
		break;
	}

	return TRUE;
}
