#include "base/async.h"

#ifdef CH_WIN32
#include "platform_win32.h"
#endif

// null on the main thread
ATTRIBUTE(thread) IThread* g_currentThread;

BASEAPI IThread* Async_CreateThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize)
{
#ifdef CH_WIN32
    return new CWindowsThread(start, userData, name, stackSize, maxStackSize);
#else
    return nullptr;
#endif
}
