#include "base.h"
#include "base/async.h"

#ifdef CH_WIN32
#include "platform_win32.h"
#endif

BASEAPI IMutex* Async_CreateMutex()
{
	ASSERT_MSG(g_platInitialized != false, "Call Base_Init first!");

#ifdef CH_WIN32
    return new CWindowsMutex();
#else
    return nullptr;
#endif
}

BASEAPI IThread* Async_CreateThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize)
{
	ASSERT_MSG(g_platInitialized != false, "Call Base_Init first!");

#ifdef CH_WIN32
    return new CWindowsThread(start, userData, name, stackSize, maxStackSize);
#else
    return nullptr;
#endif
}
