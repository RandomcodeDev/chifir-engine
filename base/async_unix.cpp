/// \file Implements Unix async support
/// \copyright Randomcode Developers

#include <pthread.h>
#include <unistd.h>

#include "base.h"
#include "base/async.h"
#include "base/basicstr.h"
#include "platform_unix.h"

CUnixMutex::CUnixMutex() : m_handle(PTHREAD_MUTEX_INITIALIZER)
{
    s32 result = pthread_mutex_init(&m_handle, nullptr);
    if (result != 0)
    {
        Base_Quit("pthread_mutex_init failed: error %d", result);
    }
}

CUnixMutex::~CUnixMutex()
{
    pthread_mutex_destroy(&m_handle);
}

void CUnixMutex::Lock()
{
    ASSERT(pthread_mutex_lock(&m_handle) == 0);
}

bool CUnixMutex::TryLock(u32 timeout)
{
    timespec delay = {};
    // ms to ns
    delay.tv_nsec = timeout * 1000000;
    s32 result = pthread_mutex_timedlock(&m_handle, &delay);
    if (result != 0)
    {
        return false;
    }

    return true;
}

void CUnixMutex::Unlock()
{
    pthread_mutex_unlock(&m_handle);
}

CUnixThread::CUnixThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize) : m_handle(0), m_alive(false), m_id(-1), m_result(0), m_name(nullptr), m_start(start), m_userData(userData)
{
    pthread_attr_t attr = {};
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stackSize);

    s32 result = pthread_create(&m_handle, &attr, reinterpret_cast<void*(*)(void*)>(ThreadMain), m_userData);
    if (result != 0)
    {
        Base_Quit("pthread_create failed: error %d", result);
    }

    if (name)
    {
        m_name = Base_StrClone(name);
    }
}

CUnixThread::~CUnixThread()
{
    m_handle = 0;

    if (m_name)
    {
        Base_Free(m_name);
    }
}

void CUnixThread::Run()
{
    m_alive = true;
}

bool CUnixThread::Wait(u32 timeout)
{
    return pthread_join(m_handle, nullptr) == 0;
}

thread_local IThread* g_currentThread;
thread_local bool g_isMainThread;

void* CUnixThread::ThreadMain(CUnixThread* thread)
{
#ifdef CH_LINUX
    if (thread->m_name)
    {
        pthread_setname_np(thread->m_handle, thread->m_name);
    }

    thread->m_id = gettid();
    g_currentThread = thread;
    g_isMainThread = false;
#endif

    // yield until Run() is called
    while (!thread->m_alive)
    {
        sched_yield();
    }

    thread->m_result = thread->m_start(thread->m_userData);

    return nullptr;
}

BASEAPI IThread* Async_GetCurrentThread()
{
    return g_currentThread;
}

BASEAPI bool Async_IsMainThread()
{
    return g_isMainThread;
}

BASEAPI u64 Async_GetCurrentThreadId()
{
    if (g_currentThread)
    {
        return g_currentThread->GetId();
    }

#ifdef CH_LINUX
    return gettid();
#endif
}
