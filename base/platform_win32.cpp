#include "platform_win32.h"
#include "base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"

DECLARE_AVAILABLE(DbgPrint);
DECLARE_AVAILABLE(NtAllocateVirtualMemory);
DECLARE_AVAILABLE(RtlTimeToTimeFields);
#ifdef CH_XBOX360
extern "C" DLLIMPORT void __stdcall XamTerminateTitle();
extern "C" DLLIMPORT u8* XboxKrnlVersion;
extern "C" DLLIMPORT u32* XboxHardwareInfo;
#else
DECLARE_AVAILABLE(NtTerminateProcess);
DECLARE_AVAILABLE(AllocConsole);
DECLARE_AVAILABLE(AttachConsole);
DECLARE_AVAILABLE(GetConsoleMode);
DECLARE_AVAILABLE(GetStdHandle);
DECLARE_AVAILABLE(NtWriteFile);
DECLARE_AVAILABLE(SetConsoleMode);
#endif

SYSTEM_BASIC_INFORMATION g_systemInfo;
SYSTEM_PERFORMANCE_INFORMATION g_systemPerfInfo;

static char* s_systemDescription;
static char* s_hardwareDescription;

#ifndef CH_XBOX360
static bool HaveNewConsole()
{
	// 10.0.10586 (version 1511)
	return USER_SHARED_DATA->NtMajorVersion >= 10 && USER_SHARED_DATA->NtBuildNumber >= 10586;
}

BASEAPI void Plat_WriteConsole(cstr text)
{
	u32 length = static_cast<u32>(Base_StrLength(text));
	IO_STATUS_BLOCK ioStatus = {};
	if (NtWriteFile_Available() && GetStdHandle_Available())
	{
		NtWriteFile(GetStdHandle(STD_OUTPUT_HANDLE), nullptr, nullptr, nullptr, &ioStatus, (dstr)text, length, nullptr, nullptr);
	}
}
#endif

BASEAPI void Plat_Init()
{
	// This is to allow the loader to use memory allocation for library names
#ifdef CH_XBOX360
	// 64K pages are the default
	g_systemInfo.PageSize = 0x10000;
#else
	// Every other supported architecture uses 4K pages
	g_systemInfo.PageSize = 0x1000;
#endif
	g_allocUsable = true;

	if (!Base_InitLoader())
	{
		Base_AbortSafe(LastNtStatus(), "Failed to initialize dynamic loader");
	}

#ifndef CH_XBOX360
	// Properly determine the page size just in case, and get other info
	NTSTATUS status = NtQuerySystemInformation(SystemBasicInformation, &g_systemInfo, SIZEOF(SYSTEM_BASIC_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		Base_Abort(status, "Failed to get basic system information: NTSTATUS 0x%08X", status);
	}

	// Need this for RAM available
	status = NtQuerySystemInformation(
		SystemPerformanceInformation, &g_systemPerfInfo, SIZEOF(SYSTEM_PERFORMANCE_INFORMATION), nullptr);
	if (!NT_SUCCESS(status))
	{
		Base_Abort(status, "Failed to get system performance information: NTSTATUS 0x%08X", status);
	}
#endif

	// Initialize these in advance
	(void)Plat_GetSystemDescription();
	(void)Plat_GetHardwareDescription();

#ifndef CH_XBOX360
#ifdef CH_WIN32
	// Get a console
	bool haveConsole = false;
	if (AttachConsole_Available())
	{
		haveConsole = AttachConsole(ATTACH_PARENT_PROCESS);
		if (haveConsole)
		{
			Plat_WriteConsole("\n");
		}
		else if (AllocConsole_Available())
		{
#ifndef CH_RETAIL
			AllocConsole();
			haveConsole = true;
#endif
		}
	}
#endif

	if (haveConsole && HaveNewConsole() && GetConsoleMode_Available() && SetConsoleMode_Available())
	{
		DWORD mode = 0;
		GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
		SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
#endif

	g_platInitialized = true;
}

BASEAPI void Plat_Shutdown()
{
	if (s_systemDescription)
	{
		Base_Free(s_systemDescription);
	}
	if (s_hardwareDescription)
	{
		Base_Free(s_hardwareDescription);
	}
}

BASEAPI cstr Plat_GetSystemDescription()
{
	if (!s_systemDescription)
	{
#ifdef CH_XBOX360
		// TODO: figure this out, probably easy
		s_systemDescription = Base_StrFormat(
			"Xbox 360 (kernel %u.%u.%u.%u)", XboxKrnlVersion[0] << 16 | XboxKrnlVersion[1],
			XboxKrnlVersion[2] << 16 | XboxKrnlVersion[3], XboxKrnlVersion[4] << 16 | XboxKrnlVersion[5], XboxKrnlVersion[6],
			XboxKrnlVersion[7]);
#else
		s_systemDescription = Base_StrFormat(
			"Windows %u.%u.%u (reported as %u.%u.%u)", USER_SHARED_DATA->NtMajorVersion, USER_SHARED_DATA->NtMinorVersion,
			USER_SHARED_DATA->NtBuildNumber, NtCurrentPeb()->OSMajorVersion, NtCurrentPeb()->OSMinorVersion,
			NtCurrentPeb()->OSBuildNumber);
#endif
	}

	return s_systemDescription;
}

BASEAPI cstr Plat_GetHardwareDescription()
{
	if (!s_hardwareDescription)
	{
#ifdef CH_XBOX360
		// TODO: figure this out, should be easy
		s_hardwareDescription = Base_StrFormat("XboxHardwareInfo 0x%X%X", XboxHardwareInfo[0], XboxHardwareInfo[1]);
#else
#ifdef CH_X86
		s64 freeMemory = g_systemPerfInfo.AvailablePages * static_cast<s64>(g_systemInfo.PageSize);
		s64 physicalMemory = g_systemInfo.NumberOfPhysicalPages * static_cast<s64>(g_systemInfo.PageSize);

		// TODO: do stuff with the funny numbers instead of relying on the name being given directly
		s_hardwareDescription = Base_StrFormat(
			"%s %s with %s of RAM (%s free)", g_cpuData.brand, g_cpuData.haveName ? g_cpuData.name : "Unknown",
			Base_FormatSize(physicalMemory).Data(), Base_FormatSize(freeMemory).Data());
#else
		s_hardwareDescription = Base_StrClone("");
#endif
#endif
	}

	return s_hardwareDescription;
}

BASEAPI NORETURN void Base_AbortSafe(s32 code, cstr msg)
{
	if (code == 1)
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

	if (DbgPrint_Available())
	{
		DbgPrint("Fatal error: %s\n", msg);
	}

	// TODO: use NtRaiseHardError

	BREAKPOINT();
#ifdef CH_XBOX360
	XamTerminateTitle();
#else
	if (NtTerminateProcess_Available())
	{
		NtTerminateProcess(NtCurrentProcess(), static_cast<NTSTATUS>(code));
	}
#endif

	ASSUME(0);
}

bool Base_GetSystemMemory(ssize size)
{
	// Linked list nodes, can contain any size of allocation, but there's a limit to the number of OS allocations
	static LinkedNode_t<SystemAllocation_t> memoryNodes[64];

	if (!NtAllocateVirtualMemory_Available())
	{
		return false;
	}

	// Maximize available space by rounding up to page size directly
	size = ALIGN(size, g_systemInfo.PageSize);

	ASSERT_MSG(
		g_memInfo.allocations.Size() < ARRAY_SIZE(memoryNodes),
		"OS allocation nodes exhausted, increase the size of the memory nodes array");

	LinkedNode_t<SystemAllocation_t>* node = &memoryNodes[g_memInfo.allocations.Size()];
	node->data.size = size;

	NTSTATUS status =
		NtAllocateVirtualMemory(NtCurrentProcess(), &node->data.memory, 0, (PSIZE_T)&node->data.size, MEM_COMMIT, PAGE_READWRITE);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		return false;
	}

	g_memInfo.size += size;
	g_memInfo.allocations.Append(node);

	return true;
}

void Base_ReleaseSystemMemory(LinkedNode_t<SystemAllocation_t>* allocation)
{
	ssize size = 0;
	NtFreeVirtualMemory(NtCurrentProcess(), &allocation->data.memory, (PSIZE_T)&size, MEM_RELEASE);
	g_memInfo.allocations.Remove(allocation);
}

void Base_ReleaseAllMemory()
{
	for (LinkedNode_t<SystemAllocation_t>* cur = g_memInfo.allocations.GetHead(); cur; cur = cur->GetNext())
	{
		Base_ReleaseSystemMemory(cur);
	}
}

DECLARE_AVAILABLE(DbgPrint);

BASEAPI void CDbgPrintLogWriter::Write(const LogMessage_t& message)
{
	if (DbgPrint_Available())
	{
		DbgPrint(LOG_FORMAT(false, message));
	}
}

#ifndef CH_XBOX360
void CWin32ConsoleLogWriter::Write(const LogMessage_t& message)
{
	dstr fullMessage = Base_StrFormat(LOG_FORMAT(HaveNewConsole(), message));
	Plat_WriteConsole(fullMessage);
	Base_Free(fullMessage);
}
#endif

static u64 GetSysTime(s64* timeZoneBias = nullptr)
{
#ifdef CH_XBOX360
	// idk
#else
	LARGE_INTEGER time;
	do
	{
		time.HighPart = USER_SHARED_DATA->SystemTime.High1Time;
		time.LowPart = USER_SHARED_DATA->SystemTime.LowPart;
	} while (time.HighPart != USER_SHARED_DATA->SystemTime.High2Time);

	if (timeZoneBias)
	{
		PLARGE_INTEGER timeZone = reinterpret_cast<PLARGE_INTEGER>(timeZoneBias);
		do
		{
			timeZone->HighPart = USER_SHARED_DATA->TimeZoneBias.High1Time;
			timeZone->LowPart = USER_SHARED_DATA->TimeZoneBias.LowPart;
		} while (timeZone->HighPart != USER_SHARED_DATA->TimeZoneBias.High2Time);
	}

	return time.QuadPart;
#endif
}

BASEAPI u64 Plat_GetMilliseconds()
{
	return GetSysTime() / 10000;
}

BASEAPI void Plat_GetDateTime(DateTime_t& time, bool utc)
{
	s64 timeZoneBias = 0;
	LARGE_INTEGER ntTime;
	ntTime.QuadPart = GetSysTime(&timeZoneBias);
	if (!utc)
	{
		ntTime.QuadPart -= timeZoneBias;
	}

	TIME_FIELDS fields = {};
	if (RtlTimeToTimeFields_Available())
	{
		RtlTimeToTimeFields(&ntTime, &fields);

		time.hour = fields.Hour;
		time.minute = fields.Minute;
		time.second = fields.Second;
		time.millisecond = fields.Milliseconds;

		time.year = fields.Year;
		time.month = fields.Month;
		time.day = fields.Day;
		time.weekDay = fields.Weekday;
	}
}
