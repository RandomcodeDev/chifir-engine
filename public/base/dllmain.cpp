#include "base/platform.h"

extern "C" BOOL __stdcall _DllMainCRTStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
    (void)hDllHandle;
    (void)dwReason;
    (void)lpreserved;
    return TRUE;
}
