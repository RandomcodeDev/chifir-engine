#include "base.h"
#include "base/async.h"
#include "base/basicstr.h"
#include "platform_win32.h"

CWindowsMutex::CWindowsMutex() : m_handle(nullptr)
{
	NTSTATUS status = NtCreateMutant(&m_handle, MUTANT_ALL_ACCESS, nullptr, FALSE);
	if (!NT_SUCCESS(status))
	{
		Base_Quit("Failed to create mutant: NTSTATUS 0x%08X", status);
	}
}

CWindowsMutex::~CWindowsMutex()
{
	if (m_handle)
	{
		NtClose(m_handle);
		m_handle = nullptr;
	}
}

void CWindowsMutex::Lock()
{
	TryLock(UINT32_MAX);
}

bool CWindowsMutex::TryLock(u32 timeout)
{
	LARGE_INTEGER delay;
	delay.QuadPart = -10000 * timeout;
	NTSTATUS status = NtWaitForSingleObject(m_handle, false, &delay);
	switch (status)
	{
		// TODO: not sure if this is correct, guess I'll find out eventually
	case STATUS_ABANDONED:
	case STATUS_WAIT_0:
		return true;
	default:
		return false;
	}
}

void CWindowsMutex::Unlock()
{
	NtReleaseMutant(m_handle, nullptr);
}

#define PS_ATTRIBUTE_LIST_SIZE(n) (sizeof(PS_ATTRIBUTE_LIST) + ((n) - 1) * sizeof(PS_ATTRIBUTE))

CWindowsThread::CWindowsThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize)
	: m_handle(nullptr), m_id(0), m_result(INT32_MIN), m_name(nullptr), m_start(start), m_userData(userData)
{
	ASSERT(stackSize <= maxStackSize);

	u8 psAttrBuf[PS_ATTRIBUTE_LIST_SIZE(1)] = {};
	PPS_ATTRIBUTE_LIST psAttrs = reinterpret_cast<PPS_ATTRIBUTE_LIST>(psAttrBuf);
	psAttrs->TotalLength = sizeof(PS_ATTRIBUTE_LIST);

	CLIENT_ID clientId = {};
	PPS_ATTRIBUTE clientIdAttr = &psAttrs->Attributes[0];
	clientIdAttr->Attribute = PS_ATTRIBUTE_CLIENT_ID;
	clientIdAttr->Size = sizeof(CLIENT_ID);
	clientIdAttr->ValuePtr = &clientId;

	OBJECT_ATTRIBUTES objAttrs = {};
	InitializeObjectAttributes(&objAttrs, nullptr, 0, nullptr, nullptr);

	NTSTATUS status = NtCreateThreadEx(
		&m_handle, THREAD_ALL_ACCESS, &objAttrs, NtCurrentProcess(), reinterpret_cast<PUSER_THREAD_START_ROUTINE>(ThreadMain),
		this, THREAD_CREATE_FLAGS_CREATE_SUSPENDED, 0, stackSize, maxStackSize, psAttrs);
	if (!NT_SUCCESS(status))
	{
		Base_Quit(
			"Failed to create thread with start 0x%016X and user data 0x%016X: NTSTATUS 0x%08X", m_start, m_userData, status);
	}

	m_id = reinterpret_cast<u64>(clientId.UniqueThread);

	if (name)
	{
		m_name = Base_StrClone(name);
	}
}

CWindowsThread::~CWindowsThread()
{
	if (m_name)
	{
		Base_Free(m_name);
		m_name = nullptr;
	}

	if (m_handle)
	{
		NtClose(m_handle);
		m_handle = nullptr;
	}
}

void CWindowsThread::Run()
{
	NTSTATUS status = NtResumeThread(m_handle, nullptr);
	if (!NT_SUCCESS(status))
	{
		Base_Quit("Failed to resume thread 0x%016X: NTSTATUS 0x%08X", m_handle, status);
	}
}

bool CWindowsThread::Wait(u32 timeout)
{
	LARGE_INTEGER delay;
	// negative means relative, and timeout is in 100ns intervals
	delay.QuadPart = timeout * -10000ll;
	NTSTATUS status = NtWaitForSingleObject(m_handle, false, &delay);
	if (status == STATUS_TIMEOUT)
	{
		return false;
	}

	// the thread's result is stored in ThreadMain

	return true;
}

u32 g_tlsIndex;

TlsData* Plat_GetTlsData()
{
	return static_cast<TlsData*>(TlsGetValue(g_tlsIndex));
}

NTSTATUS NTAPI CWindowsThread::ThreadMain(CWindowsThread* thread)
{
	TlsData* data = new TlsData;

	data->currentThread = thread;
	data->isMainThread = false;
	TlsSetValue(g_tlsIndex, data);

	thread->m_result = thread->m_start(thread->m_userData);
	return thread->m_result;
}

BASEAPI IThread* Async_GetCurrentThread()
{
	TlsData* data = Plat_GetTlsData();
	if (data)
	{
		return data->currentThread;
	}

	return nullptr;
}

BASEAPI bool Async_IsMainThread()
{
	TlsData* data = Plat_GetTlsData();
	if (data)
	{
		return data->isMainThread;
	}

	return false;
}

BASEAPI u64 Async_GetCurrentThreadId()
{
	IThread* current = Async_GetCurrentThread();
	if (current)
	{
		// skip over doing a syscall
		return current->GetId();
	}

	THREAD_BASIC_INFORMATION info = {};
	NTSTATUS status =
		NtQueryInformationThread(NtCurrentThread(), ThreadBasicInformation, &info, sizeof(THREAD_BASIC_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Failed to get thread ID for current thread: NTSTATUS 0x%08X\n", status);
		return UINT64_MAX;
	}

	return reinterpret_cast<u64>(info.ClientId.UniqueThread);
}
