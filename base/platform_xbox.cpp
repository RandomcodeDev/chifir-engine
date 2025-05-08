/// \file <PLATFORM NAME> support
/// \copyright 2025 Randomcode Developers

#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"

#include "base.h"
#include "platform_xbox.h"

static dstr s_systemDescription;
static dstr s_hardwareDescription;
static char s_titleDataDrive[3];

BASEAPI void Plat_Init()
{
	if (!g_platInitialized)
	{
		g_allocUsable = true;

#ifdef CH_XENON
		g_tlsIndex = TlsAlloc();
#endif
		Plat_GetTlsData()->isMainThread = true;

		DWORD error = XMountAlternateTitleA("T:\\", 0, &s_titleDataDrive[0]);
		if (error != ERROR_SUCCESS)
		{
			Base_Quit("Failed to create save game: Win32 error %d", error);
		}

		g_platInitialized = true;
	}
}

BASEAPI void Plat_Shutdown()
{
	if (g_platInitialized)
	{
		if (s_systemDescription)
		{
			Base_Free(s_systemDescription);
		}

		if (s_hardwareDescription)
		{
			Base_Free(s_hardwareDescription);
		}

		XUnmountAlternateTitleA(s_titleDataDrive[0]);

		g_platInitialized = false;
	}
}

BASEAPI void Plat_GetArgs(CVector<CString>& args)
{
}

BASEAPI cstr Plat_GetSystemDescription()
{
	if (!s_systemDescription)
	{
		s_systemDescription = Base_StrFormat(
			"Xbox kernel %u.%u.%u.%u", XboxKrnlVersion->Major, XboxKrnlVersion->Minor, XboxKrnlVersion->Build,
			XboxKrnlVersion->Qfe);
	}

	return s_systemDescription;
}

BASEAPI cstr Plat_GetHardwareDescription()
{
	if (!s_hardwareDescription)
	{
		// TODO: is this the same on Xenon?
		s_hardwareDescription = Base_StrFormat(
			"Xbox%s with GPU revision %u and MCPX revision %u",
			(XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) ? " devkit" : "", XboxHardwareInfo->GpuRevision,
			XboxHardwareInfo->McpRevision);
	}

	return s_hardwareDescription;
}

#ifndef CH_XENON
static ULONG HardError(PCUNICODE_STRING msg, HARDERROR_RESPONSE_OPTION option = OptionAbortRetryIgnore)
{
	UNICODE_STRING titleStr;
	RtlInitUnicodeString(&titleStr, L"Fatal error!");
	ULONG_PTR params[] = {reinterpret_cast<ULONG_PTR>(msg), reinterpret_cast<ULONG_PTR>(&titleStr), 0, INFINITE};
	ULONG response = 0;
	NtRaiseHardError(
		HARDERROR_OVERRIDE_ERRORMODE | STATUS_SERVICE_NOTIFICATION, (ULONG)ArraySize(params), 0b0011, params, option, &response);

	return response;
}
#endif

BASEAPI NORETURN void Base_AbortSafe(s32 code, cstr msg)
{
	if (code == ABORT_RELEVANT_ERROR)
	{
		if (LastNtStatus() != 0)
		{
			code = LastNtStatus();
		}
		else if (LastNtError() != 0)
		{
			code = LastNtError();
		}
		else
		{
			code = STATUS_FATAL_APP_EXIT;
		}
	}

	OutputDebugStringA("Fatal error: ");
	OutputDebugStringA(msg);
	OutputDebugStringA("\n");

#ifdef CH_XENON
	XamTerminateTitle();
#else
	abort();
#endif

	UNREACHABLE();
}

bool Base_GetSystemMemory(ssize size)
{
	// Linked list nodes, can contain any size of allocation, but there's a limit to the number of OS allocations
	static IntrusiveLinkedNode<SystemAllocation> memoryNodes[64];

	// Maximize available space by rounding up to page size directly
	size = AlignUp(size, 0x1000);

	ASSERT_MSG(
		g_memInfo.allocations.Size() < ArraySize(memoryNodes),
		"OS allocation nodes exhausted, increase the size of the memory nodes array");

	IntrusiveLinkedNode<SystemAllocation>* node = &memoryNodes[g_memInfo.allocations.Size()];
	node->data.size = size;

	node->data.memory = VirtualAlloc(nullptr, node->data.size, MEM_COMMIT, PAGE_READWRITE);
	if (!node->data.memory)
	{
		return false;
	}

	g_memInfo.size += size;
	g_memInfo.allocations.Append(node);

	return true;
}

BASEAPI ILibrary* Base_LoadLibrary(cstr name)
{
	return nullptr;
}

CXboxLibrary::CXboxLibrary(cstr name, void* base) : m_name(Base_StrClone(name)), m_base(base)
{
}

CXboxLibrary::~CXboxLibrary()
{
}

void CXboxLibrary::Unload()
{
}

void* CXboxLibrary::GetSymbol(cstr name)
{
	return nullptr;
}

BASEAPI bool Plat_ConsoleHasColor()
{
	return false;
}

BASEAPI void Plat_WriteConsole(cstr message)
{
	OutputDebugStringA(message);
}

BASEAPI cstr Plat_GetSaveLocation()
{
	return s_titleDataDrive;
}

BASEAPI cstr Plat_GetEngineDir()
{
	return "D:\\";
}

BASEAPI cstr Plat_GetEnvironment(cstr name)
{
	return getenv(name);
}

BASEAPI u64 Plat_GetMilliseconds()
{
	LARGE_INTEGER pc;
	LARGE_INTEGER freq;
	QueryPerformanceCounter(&pc);
	QueryPerformanceFrequency(&freq);
	pc.QuadPart *= 1000000; // to micros
	pc.QuadPart /= freq.QuadPart;
	pc.QuadPart *= 1000; // to millis
	return pc.QuadPart;
}

BASEAPI void Plat_GetDateTime(DateTime& time, bool utc)
{
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);

	time.year = sysTime.wYear;
	time.month = sysTime.wMonth;
	time.day = sysTime.wDay;
	time.weekDay = sysTime.wDayOfWeek;

	time.hour = sysTime.wHour;
	time.minute = sysTime.wMinute;
	time.second = sysTime.wSecond;
	time.millisecond = sysTime.wMilliseconds;
}
