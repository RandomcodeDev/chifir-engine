#include "base.h"
#include "base/async.h"
#include "base/basicstr.h"
#include "platform_xbox.h"

CXboxMutex::CXboxMutex() : m_handle(nullptr)
{
	m_handle = CreateMutexA(nullptr, false, nullptr);
	if (!m_handle)
	{
		Base_Quit("Failed to create mutex: Win32 error %d", GetLastError());
	}
}

CXboxMutex::~CXboxMutex()
{
	if (m_handle)
	{
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
}

void CXboxMutex::Lock()
{
	TryLock(UINT32_MAX);
}

bool CXboxMutex::TryLock(u32 timeout)
{
	DWORD status = WaitForSingleObject(m_handle, timeout);
	switch (status)
	{
		// TODO: not sure if this is correct, guess I'll find out eventually
	case WAIT_ABANDONED:
	case WAIT_OBJECT_0:
		return true;
    case WAIT_TIMEOUT:
    case WAIT_FAILED:
	default:
		return false;
	}
}

void CXboxMutex::Unlock()
{
	ReleaseMutex(m_handle);
}

CXboxThread::CXboxThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize)
	: m_handle(nullptr), m_id(0), m_result(INT32_MIN), m_name(nullptr), m_start(start), m_userData(userData)
{
	ASSERT(stackSize <= maxStackSize);

	CreateThread(nullptr, stackSize, reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadMain), this, CREATE_SUSPENDED, nullptr);

	if (name)
	{
		m_name = Base_StrClone(name);
	}
}

CXboxThread::~CXboxThread()
{
	if (m_name)
	{
		Base_Free(m_name);
		m_name = nullptr;
	}

	if (m_handle)
	{
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
}

void CXboxThread::Run()
{
	ResumeThread(m_handle);
}

bool CXboxThread::Wait(u32 timeout)
{
	DWORD status = WaitForSingleObject(m_handle, timeout);
	if (status == WAIT_TIMEOUT)
	{
		return false;
	}

	// the thread's result is stored in ThreadMain

	return true;
}

u32 g_tlsIndex;

TlsData* Plat_GetTlsData()
{
	TlsData* data = static_cast<TlsData*>(TlsGetValue(g_tlsIndex));
	if (!data)
	{
		data = new TlsData;

		data->currentThread = nullptr;
		data->isMainThread = false;
		TlsSetValue(g_tlsIndex, data);
	}

	return data;
}

LONG WINAPI CXboxThread::ThreadMain(CXboxThread* thread)
{
	Plat_GetTlsData()->currentThread = thread;
	Plat_GetTlsData()->isMainThread = false;
	thread->m_result = thread->m_start(thread->m_userData);
	return thread->m_result;
}

BASEAPI IThread* Async_GetCurrentThread()
{
	return Plat_GetTlsData()->currentThread;
}

BASEAPI bool Async_IsMainThread()
{
	return Plat_GetTlsData()->isMainThread;
}

BASEAPI u64 Async_GetCurrentThreadId()
{
	IThread* current = Async_GetCurrentThread();
	if (current)
	{
		return current->GetId();
	}

    return GetCurrentThreadId();
}

BASEAPI void Async_Yield()
{
	Yield();
}
