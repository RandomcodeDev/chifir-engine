/// \file Shared library startup/shutdown
/// \copyright Randomcode Developers

#include "base/platform.h"

#ifdef CH_WIN32
extern "C" BOOL __stdcall _DllMainCRTStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
	UNUSED(hDllHandle);
	UNUSED(lpreserved);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		__security_init_cookie();
		RunGlobalConstructors();
		//__dyn_tls_init(nullptr, DLL_THREAD_ATTACH, nullptr);
#ifdef IN_BASE
		Base_Init();
#endif
		break;
	case DLL_THREAD_ATTACH:
		//__dyn_tls_init(nullptr, DLL_THREAD_ATTACH, nullptr);
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
#elif defined CH_UNIX
ATTRIBUTE(constructor) void DllStartup()
{
#ifdef IN_BASE
	Base_Init();
#endif
}

ATTRIBUTE(destructor) void DllShutdown()
{
#ifdef IN_BASE
	Base_Shutdown();
#endif
}
#endif
