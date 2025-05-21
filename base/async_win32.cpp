#include "base.h"
#include "base/async.h"
#include "base/basicstr.h"
#include "platform_win32.h"

DECLARE_AVAILABLE(NtQueryInformationThread);
DECLARE_AVAILABLE(TlsGetValue);

CWindowsMutex::CWindowsMutex() : m_handle(nullptr)
{
	NTSTATUS status = NtCreateMutant(
		&m_handle, MUTANT_ALL_ACCESS, nullptr
		,
		FALSE
	);
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

#define PS_ATTRIBUTE_LIST_SIZE(n) (SIZEOF(PS_ATTRIBUTE_LIST) + ((n) - 1) * SIZEOF(PS_ATTRIBUTE))

void CWindowsThread::CreateVistaThread(ssize stackSize, ssize maxStackSize)
{
	u8 psAttrBuf[PS_ATTRIBUTE_LIST_SIZE(1)] = {};
	PPS_ATTRIBUTE_LIST psAttrs = reinterpret_cast<PPS_ATTRIBUTE_LIST>(psAttrBuf);
	psAttrs->TotalLength = SIZEOF(PS_ATTRIBUTE_LIST);

	CLIENT_ID clientId = {};
	PPS_ATTRIBUTE clientIdAttr = &psAttrs->Attributes[0];
	clientIdAttr->Attribute = PS_ATTRIBUTE_CLIENT_ID;
	clientIdAttr->Size = SIZEOF(CLIENT_ID);
	clientIdAttr->ValuePtr = &clientId;

	OBJECT_ATTRIBUTES objAttrs = {};
	InitializeObjectAttributes(&objAttrs, nullptr, 0, nullptr, nullptr);

	NTSTATUS status = NtCreateThreadEx(
		&m_handle, THREAD_ALL_ACCESS, &objAttrs, NtCurrentProcess(), reinterpret_cast<PUSER_THREAD_START_ROUTINE>(ThreadMain),
		this, THREAD_CREATE_FLAGS_CREATE_SUSPENDED, 0, stackSize, maxStackSize, psAttrs);
	if (!NT_SUCCESS(status))
	{
		Base_Quit("NtCreateThreadEx with start 0x%016zX and user data 0x%016zX: NTSTATUS 0x%08X", m_start, m_userData, status);
	}

	m_id = reinterpret_cast<u64>(clientId.UniqueThread);
}

// Based on ReactOS's BaseCreateStack
void CWindowsThread::CreateStack(ssize size, ssize maxSize, PINITIAL_TEB InitialTeb)
{
	NTSTATUS Status;
	PIMAGE_NT_HEADERS Headers;
	ULONG_PTR Stack;
	BOOLEAN UseGuard;
	ULONG PageSize, AllocationGranularity, Dummy;
	SIZE_T MinimumStackCommit, GuardPageSize;

	/* Read page size */
	PageSize = g_systemInfo.PageSize;
	AllocationGranularity = g_systemInfo.AllocationGranularity;

	/* Get the Image Headers */
	auto DosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(NtCurrentPeb()->ImageBaseAddress);
	Headers = (PIMAGE_NT_HEADERS)RVA_TO_VA(DosHeader, DosHeader->e_lfanew);

	SIZE_T StackReserve = maxSize;
	if (StackReserve == 0)
		StackReserve = Headers->OptionalHeader.SizeOfStackReserve;

	SIZE_T StackCommit = size;
	if (StackCommit == 0)
	{
		StackCommit = Headers->OptionalHeader.SizeOfStackCommit;
	}
	/* Check if the commit is higher than the reserve */
	else if (StackCommit >= StackReserve)
	{
		/* Grow the reserve beyond the commit, up to 1MB alignment */
		StackReserve = ALIGN_UP(StackCommit, 1024 * 1024);
	}

	/* Align everything to Page Size */
	StackCommit = ALIGN_UP(StackCommit, PageSize);
	StackReserve = ALIGN_UP(StackReserve, AllocationGranularity);

	MinimumStackCommit = NtCurrentPeb()->MinimumStackCommit;
	if ((MinimumStackCommit != 0) && (StackCommit < MinimumStackCommit))
	{
		StackCommit = MinimumStackCommit;
	}

	/* Check if the commit is higher than the reserve */
	if (StackCommit >= StackReserve)
	{
		/* Grow the reserve beyond the commit, up to 1MB alignment */
		StackReserve = ALIGN_UP(StackCommit, 1024 * 1024);
	}

	/* Align everything to Page Size */
	StackCommit = ALIGN_UP(StackCommit, PageSize);
	StackReserve = ALIGN_UP(StackReserve, AllocationGranularity);

	/* Reserve memory for the stack */
	Stack = 0;
	Status = NtAllocateVirtualMemory(NtCurrentProcess(), (PVOID*)&Stack, 0, &StackReserve, MEM_RESERVE, PAGE_READWRITE);
	if (!NT_SUCCESS(Status))
	{
		Base_Quit("Failed to reserve stack: NTSTATUS 0x%08X", Status);
	}

	/* Now set up some basic Initial TEB Parameters */
	InitialTeb->StackAllocationBase = (PVOID)Stack;
	InitialTeb->StackBase = (PVOID)(Stack + StackReserve);
	InitialTeb->OldInitialTeb.OldStackBase = NULL;
	InitialTeb->OldInitialTeb.OldStackLimit = NULL;

	/* Update the stack position */
	Stack += StackReserve - StackCommit;

	/* Check if we can add a guard page */
	if (StackReserve >= StackCommit + PageSize)
	{
		Stack -= PageSize;
		StackCommit += PageSize;
		UseGuard = TRUE;
	}
	else
	{
		UseGuard = FALSE;
	}

	/* Allocate memory for the stack */
	Status = NtAllocateVirtualMemory(NtCurrentProcess(), (PVOID*)&Stack, 0, &StackCommit, MEM_COMMIT, PAGE_READWRITE);
	if (!NT_SUCCESS(Status))
	{
		Base_Quit("Failed to allocate stack: NTSTATUS 0x%08X", Status);
	}

	/* Now set the current Stack Limit */
	InitialTeb->StackLimit = (PVOID)Stack;

	/* Create a guard page if needed */
	if (UseGuard)
	{
		GuardPageSize = PageSize;
		Status = NtProtectVirtualMemory(NtCurrentProcess(), (PVOID*)&Stack, &GuardPageSize, PAGE_GUARD | PAGE_READWRITE, &Dummy);
		if (!NT_SUCCESS(Status))
		{
			Base_Quit("Failed to create guard page: NTSTATUS 0x%08X", Status);
		}

		/* Update the Stack Limit keeping in mind the Guard Page */
		InitialTeb->StackLimit = (PVOID)((ULONG_PTR)InitialTeb->StackLimit + GuardPageSize);
	}

	m_stackAlloc = reinterpret_cast<void*>(Stack);
	m_stackSize = StackReserve;
}

// Based on ReactOS CreateRemoteThread
void CWindowsThread::CreateXpThread(ssize stackSize, ssize maxStackSize)
{
	CLIENT_ID clientId = {};
	clientId.UniqueProcess = NtCurrentProcess();

	INITIAL_TEB initialTeb = {};
	CreateStack(stackSize, maxStackSize, &initialTeb);

	CONTEXT context = {};
	RtlInitializeContext(NtCurrentProcess(), &context, m_userData, reinterpret_cast<void*>(ThreadMain), m_stackAlloc);

	OBJECT_ATTRIBUTES objAttrs = {};
	InitializeObjectAttributes(&objAttrs, nullptr, 0, nullptr, nullptr);

	NTSTATUS status =
		NtCreateThread(&m_handle, THREAD_ALL_ACCESS, &objAttrs, NtCurrentProcess(), &clientId, &context, &initialTeb, true);
	if (!NT_SUCCESS(status))
	{
		Base_Quit(
			"NtCreateThread with start 0x%016zX and user data 0x%016zX failed: NTSTATUS 0x%08X", m_start, m_userData, status);
	}

	m_id = reinterpret_cast<u64>(clientId.UniqueThread);
}

CWindowsThread::CWindowsThread(ThreadStart_t start, void* userData, cstr name, ssize stackSize, ssize maxStackSize)
	: m_handle(nullptr), m_id(0), m_result(INT32_MIN), m_name(nullptr), m_start(start), m_userData(userData)
{
	ASSERT(stackSize <= maxStackSize);

	if (AT_LEAST_WINDOWS_VISTA())
	{
		CreateVistaThread(stackSize, maxStackSize);
	}
	else
	{
		CreateXpThread(stackSize, maxStackSize);
	}

	if (name)
	{
		m_name = Base_StrClone(name);
	}
}

CWindowsThread::~CWindowsThread()
{
	if (m_stackAlloc)
	{
		NtFreeVirtualMemory(NtCurrentProcess(), &m_stackAlloc, reinterpret_cast<PSIZE_T>(&m_stackSize), MEM_RELEASE);
		m_stackAlloc = nullptr;
	}

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
	if (!TlsGetValue_Available())
	{
		return nullptr;
	}

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

NTSTATUS NTAPI CWindowsThread::ThreadMain(CWindowsThread* thread)
{
	// calling Plat_GetTlsData for the first time allocates it
	Plat_GetTlsData()->currentThread = thread;
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
	if (!NtQueryInformationThread_Available())
	{
		return UINT64_MAX;
	}

	IThread* current = Async_GetCurrentThread();
	if (current)
	{
		// skip over doing a syscall
		return current->GetId();
	}

	THREAD_BASIC_INFORMATION info = {};
	NTSTATUS status =
		NtQueryInformationThread(NtCurrentThread(), ThreadBasicInformation, &info, SIZEOF(THREAD_BASIC_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Failed to get thread ID for current thread: NTSTATUS 0x%08X\n", status);
		return UINT64_MAX;
	}

	return reinterpret_cast<u64>(info.ClientId.UniqueThread);
}

BASEAPI void Async_Yield()
{
	NtYieldExecution();
}
