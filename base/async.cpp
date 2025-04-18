#include "base.h"
#include "base/async.h"

#ifdef CH_WIN32
#include "platform_win32.h"
#elif defined CH_UNIX
#include "platform_unix.h"
#endif

BASEAPI IMutex* Async_CreateMutex()
{
	ASSERT_MSG(g_platInitialized != false, "Call Base_Init first!");

#ifdef CH_WIN32
    return new CWindowsMutex();
#elif defined CH_UNIX
    return new CUnixMutex();
#else
    return nullptr;
#endif
}

BASEAPI IThread* Async_CreateThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize)
{
	ASSERT_MSG(g_platInitialized != false, "Call Base_Init first!");

#ifdef CH_WIN32
    return new CWindowsThread(start, userData, name, stackSize, maxStackSize);
#elif defined CH_UNIX
    return new CUnixThread(start, userData, name, stackSize, maxStackSize);
#else
    return nullptr;
#endif
}
