/// \file Windows support
/// \copyright Randomcode Developers

#include <ntstatus.h>
#include <shlobj.h>
#include <winuser.h>

#include "base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"
#include "phnt.h"
#include "phnt_ntdef.h"
#include "platform_win32.h"

DECLARE_AVAILABLE(DbgPrint);
DECLARE_AVAILABLE(NtAllocateVirtualMemory);
DECLARE_AVAILABLE(RtlTimeToTimeFields);
#ifdef CH_XBOX360
extern "C" DLLIMPORT void __stdcall XamTerminateTitle();
extern "C" DLLIMPORT u8* XboxKrnlVersion;
extern "C" DLLIMPORT u32* XboxHardwareInfo;
#else
DECLARE_AVAILABLE(NtRaiseHardError);
DECLARE_AVAILABLE(RtlAnsiStringToUnicodeString);
DECLARE_AVAILABLE(NtTerminateProcess);
DECLARE_AVAILABLE(AllocConsole);
DECLARE_AVAILABLE(AttachConsole);
DECLARE_AVAILABLE(GetConsoleMode);
DECLARE_AVAILABLE(GetStdHandle);
DECLARE_AVAILABLE(WriteConsoleA);
DECLARE_AVAILABLE(SetConsoleMode);
#endif

SYSTEM_BASIC_INFORMATION g_systemInfo;
SYSTEM_PERFORMANCE_INFORMATION g_systemPerfInfo;

static char* s_systemDescription;
static char* s_hardwareDescription;
static char* s_productName;

bool g_uwp;

BASEAPI bool Plat_ConsoleHasColor()
{
#ifdef CH_XBOX360
	return false;
#else
	// 10.0.10586 (version 1511)
	return USER_SHARED_DATA->NtMajorVersion >= 10 && USER_SHARED_DATA->NtBuildNumber >= 10586;
#endif
}

BASEAPI void Plat_WriteConsole(cstr text)
{
#ifdef CH_XBOX360
	DbgPrint("%s", text);
#else
	u32 length = static_cast<u32>(Base_StrLength(text));
	// WriteConsole is only necessary before Windows 7 or 8, in one of those they reworked the
	// console to allow NtWriteFile to work. Before that, it was pure CSR calls.
	if (WriteConsoleA_Available() && GetStdHandle_Available())
	{
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), text, length, nullptr, nullptr);
	}
#endif
}

BASEAPI void Plat_Init()
{
	if (!g_platInitialized)
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
		NTSTATUS status =
			NtQuerySystemInformation(SystemBasicInformation, &g_systemInfo, SIZEOF(SYSTEM_BASIC_INFORMATION), nullptr);
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

		g_uwp = Base_InitWinRt();

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

		if (haveConsole && Plat_ConsoleHasColor() && GetConsoleMode_Available() && SetConsoleMode_Available())
		{
			DWORD mode = 0;
			GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}
#endif

		g_platInitialized = true;
	}
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

extern "C" BASEAPI int Base_RunMain(int (*main)())
{
	if (g_uwp)
	{
		return Base_RunMainWinRt(main);
	}
	else
	{
		return main();
	}
}

BASEAPI bool Plat_IsUwpApp()
{
	return g_uwp;
}

static cstr GetProductName()
{
	if (!s_productName)
	{
		// here cause of goto
		wchar_t valueName[] = L"ProductName";
		UNICODE_STRING valueNameStr = RTL_CONSTANT_STRING(valueName);

		u8 keyInfoBuffer[128] = {};
		KEY_VALUE_FULL_INFORMATION* keyInfo = reinterpret_cast<KEY_VALUE_FULL_INFORMATION*>(keyInfoBuffer);
		keyInfo->NameLength = ArraySize<ULONG>(valueName);
		keyInfo->DataLength = ArraySize<ULONG>(keyInfoBuffer) - sizeof(KEY_VALUE_FULL_INFORMATION) - ArraySize<ULONG>(valueName);
		ULONG length = 0;

		wchar_t nameBuffer[32] = {};
		UNICODE_STRING nameStr = RTL_CONSTANT_STRING(nameBuffer);

		ANSI_STRING productNameStr = {};

		wchar_t keyName[] = L"\\Registry\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion";
		UNICODE_STRING keyNameStr = RTL_CONSTANT_STRING(keyName);
		OBJECT_ATTRIBUTES objAttrs = {};
		InitializeObjectAttributes(&objAttrs, &keyNameStr, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

		HANDLE key = nullptr;
		NTSTATUS status = NtCreateKey(&key, KEY_QUERY_VALUE, &objAttrs, 0, nullptr, REG_OPTION_NON_VOLATILE, nullptr);
		if (!NT_SUCCESS(status))
		{
			s_productName = Base_StrClone("Windows");
			goto Done;
		}

		status = NtQueryValueKey(key, &valueNameStr, KeyValueFullInformation, keyInfo, sizeof(keyInfoBuffer), &length);
		if (!NT_SUCCESS(status))
		{
			s_productName = Base_StrClone("Windows");
			goto Done;
		}

		Base_MemCopy(
			nameBuffer, reinterpret_cast<cstr>(&keyInfoBuffer[keyInfo->DataOffset]),
			Min<ssize>(nameStr.MaximumLength, keyInfo->DataLength));

		RtlUnicodeStringToAnsiString(&productNameStr, &nameStr, TRUE);
		s_productName = Base_StrClone(productNameStr.Buffer, productNameStr.Length);
		RtlFreeAnsiString(&productNameStr);

	Done:
		if (key != INVALID_HANDLE_VALUE)
		{
			NtClose(key);
		}
	}

	return s_productName;
}

BASEAPI cstr Plat_GetSystemDescription()
{
	if (!s_systemDescription)
	{
#ifdef CH_XBOX360
		s_systemDescription = Base_StrFormat(
			"Xbox 360 (kernel %u.%u.%u.%u)", XboxKrnlVersion[0] << 16 | XboxKrnlVersion[1],
			XboxKrnlVersion[2] << 16 | XboxKrnlVersion[3], XboxKrnlVersion[4] << 16 | XboxKrnlVersion[5], XboxKrnlVersion[6],
			XboxKrnlVersion[7]);
#else
		cstr name = GetProductName();
		cstr wineVersion = Base_IsWine();
		s_systemDescription = Base_StrFormat(
			"%s %u.%u.%u (reported as %u.%u.%u)%s%s%s%s", name, USER_SHARED_DATA->NtMajorVersion,
			USER_SHARED_DATA->NtMinorVersion, USER_SHARED_DATA->NtBuildNumber, NtCurrentPeb()->OSMajorVersion,
			NtCurrentPeb()->OSMinorVersion, NtCurrentPeb()->OSBuildNumber, wineVersion ? " Wine " : "",
			wineVersion ? wineVersion : "", Base_CheckWoW64() ? " WoW64" : "", Plat_IsUwpApp() ? " UWP" : "");
#endif
	}

	return s_systemDescription;
}

BASEAPI cstr Plat_GetHardwareDescription()
{
#ifdef CH_XBOX360
	s_hardwareDescription = Base_StrFormat("XboxHardwareInfo 0x%X%X", XboxHardwareInfo[0], XboxHardwareInfo[1]);
#else
#ifdef CH_X86
	s64 freeMemory = g_systemPerfInfo.AvailablePages * static_cast<s64>(g_systemInfo.PageSize);
	s64 physicalMemory = g_systemInfo.NumberOfPhysicalPages * static_cast<s64>(g_systemInfo.PageSize);

	// TODO: do stuff with the funny numbers instead of relying on the CPU name being given directly
	s_hardwareDescription = Base_StrFormat(
		"%s %s with %s of RAM (%s free)", g_cpuData.brand, g_cpuData.haveName ? g_cpuData.name : "Unknown",
		Base_FormatSize(physicalMemory).Data(), Base_FormatSize(freeMemory).Data());
#else
	// TODO: ARM?
	s_hardwareDescription = Base_StrClone("<unknown>");
#endif
#endif

	return s_hardwareDescription;
}

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

	if (DbgPrint_Available())
	{
		DbgPrint("Fatal error: %s\n", msg);
	}

	if (NtRaiseHardError_Available() && RtlAnsiStringToUnicodeString_Available())
	{
		UNICODE_STRING messageUStr = {};
		ANSI_STRING messageStr = {};
		messageStr.Buffer = CONST_CAST(dstr, msg);
		messageStr.Length = static_cast<u16>(Base_StrLength(msg));
		messageStr.MaximumLength = messageStr.Length + 1;
		RtlAnsiStringToUnicodeString(&messageUStr, &messageStr, true);

		wchar_t title[] = L"Fatal error!";
		UNICODE_STRING titleStr = RTL_CONSTANT_STRING(title);
		ULONG_PTR params[] = {
			reinterpret_cast<ULONG_PTR>(&messageUStr), reinterpret_cast<ULONG_PTR>(&titleStr), MB_ABORTRETRYIGNORE | MB_ICONERROR,
			INFINITE};
		ULONG response = 0;
		NtRaiseHardError(
			HARDERROR_OVERRIDE_ERRORMODE | STATUS_SERVICE_NOTIFICATION, ArraySize<u32>(params), 0b0011, params,
			OptionAbortRetryIgnore, &response);
		if (response == ResponseRetry)
		{
			BREAKPOINT();
		}
	}

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
	static IntrusiveLinkedNode<SystemAllocation_t> memoryNodes[64];

	if (!NtAllocateVirtualMemory_Available())
	{
		return false;
	}

	// Maximize available space by rounding up to page size directly
	size = AlignUp(size, g_systemInfo.PageSize);

	ASSERT_MSG(
		g_memInfo.allocations.Size() < ArraySize(memoryNodes),
		"OS allocation nodes exhausted, increase the size of the memory nodes array");

	IntrusiveLinkedNode<SystemAllocation_t>* node = &memoryNodes[g_memInfo.allocations.Size()];
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

void Base_ReleaseSystemMemory(IntrusiveLinkedNode<SystemAllocation_t>* allocation)
{
	ssize size = 0;
	NtFreeVirtualMemory(NtCurrentProcess(), &allocation->data.memory, (PSIZE_T)&size, MEM_RELEASE);
	g_memInfo.allocations.Remove(allocation);
}

void Base_ReleaseAllMemory()
{
	for (IntrusiveLinkedNode<SystemAllocation_t>* cur = g_memInfo.allocations.GetHead(); cur; cur = cur->GetNext())
	{
		Base_ReleaseSystemMemory(cur);
	}
}

DECLARE_AVAILABLE(DbgPrint);

BASEAPI void CDbgPrintLogWriter::Write(const LogMessage& message)
{
	if (DbgPrint_Available())
	{
		DbgPrint(LOG_FORMAT(false, message));
	}
}

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

BASEAPI void Plat_Sleep(u64 millis)
{
	LARGE_INTEGER delay;
	delay.QuadPart = millis;
	NtDelayExecution(FALSE, &delay);
}

BASEAPI void Plat_GetDateTime(DateTime& time, bool utc)
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

BASEAPI cstr Plat_GetSaveLocation()
{
	static char s_directory[MAX_PATH + 1] = {0};

	if (!Base_StrLength(s_directory))
	{
		if (g_uwp)
		{
			Base_StrCopy(s_directory, Base_GetWinRtAppData(), ArraySize(s_directory));
		}
		else
		{
			SHGetFolderPathA(nullptr, CSIDL_APPDATA | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, s_directory);
			Base_StrCopy(
				s_directory + Base_StrLength(s_directory), GAME_NAME,
				Min(ArraySize(s_directory) - Base_StrLength(s_directory), ArraySize(GAME_NAME)));
		}
		s_directory[Min(ArraySize(s_directory) - 1, Base_StrLength(s_directory))] = '/';
	}

	return s_directory;
}

BASEAPI cstr Plat_GetEngineDir()
{
	static char s_directory[MAX_PATH + 1] = {0};

	if (!Base_StrLength(s_directory))
	{
		PUNICODE_STRING unicodeImagePath = &NtCurrentPeb()->ProcessParameters->ImagePathName;
		ANSI_STRING imagePath = {};
		RtlUnicodeStringToAnsiString(&imagePath, unicodeImagePath, true);
		ssize index = Base_StrFind(imagePath.Buffer, '\\', true, imagePath.Length - 1);
		if (index < 0)
		{
			Base_StrCopy(s_directory, "./", ArraySize(s_directory));
			return s_directory;
		}

		Base_StrCopy(s_directory, imagePath.Buffer, Min(index, ArraySize(s_directory)));
		RtlFreeAnsiString(&imagePath);
	}

	return s_directory;
}

BASEAPI cstr Plat_GetEnvironment(cstr name)
{
	return nullptr;
}
