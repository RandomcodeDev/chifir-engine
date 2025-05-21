/// \file Platform API
/// \copyright 2025 Randomcode Developers

#pragma once

#ifdef CH_WIN32
#include "phnt_wrapper.h"
#include <shlobj.h>
#ifdef CH_UWP
#include "winrt_min.h"
#endif
#include "win32_aliases.h"
#elif defined CH_XBOX
#define _InterlockedCompareExchange _InterlockedCompareExchange_xbox
#define _InterlockedDecrement       _InterlockedDecrement_xbox
#define _InterlockedExchange        _InterlockedExchange_xbox
#define _InterlockedExchangeAdd     _InterlockedExchangeAdd_xbox
#define _InterlockedIncrement       _InterlockedIncrement_xbox
#include <xtl.h>
#include <xbdm.h>
#include "win32_aliases.h"
#undef _InterlockedCompareExchange
#undef _InterlockedDecrement
#undef _InterlockedExchange
#undef _InterlockedExchangeAdd
#undef _InterlockedIncrement
#elif defined CH_UNIX
#include <unistd.h>
#endif

#include "base.h"
#include "compiler.h"
#include "dll.h"
#include "types.h"

class CString;
template <typename T> class CVector;

/// Initialize anything necessary to use platform functions. Terminates the process on failure.
extern BASEAPI void Plat_Init();

/// Shut down platform
extern BASEAPI void Plat_Shutdown();

/// Get the command line arguments given to the program
extern BASEAPI void Plat_GetArgs(CVector<CString>& args);

/// Get a description of the OS
extern BASEAPI cstr Plat_GetSystemDescription();

/// Get a description of the hardware (CPU, RAM)
extern BASEAPI cstr Plat_GetHardwareDescription();

/// Write to the terminal/console (stdout)
extern BASEAPI void Plat_WriteConsole(cstr message);

/// Whether you can use ANSI escapes with Plat_WriteConsole
extern BASEAPI bool Plat_ConsoleHasColor();

/// Get a timestamp in milliseconds.
//
/// Use Plat_GetDateTime for real world time, this is only for timing and not relative to a consistent point across platforms.
extern BASEAPI u64 Plat_GetMilliseconds();

/// Get the date and time
extern BASEAPI void Plat_GetDateTime(DateTime& time, bool utc = false);

/// Get the location for saving engine data (logs, etc)
extern BASEAPI cstr Plat_GetDataLocation();

/// Get the location the engine is running from
extern BASEAPI cstr Plat_GetEngineDir();

/// Get an environment variable
extern BASEAPI cstr Plat_GetEnvironment(cstr name);

/// Get whether a Ctrl-C or similar quit signal was received
extern BASEAPI bool Plat_QuitSignalled();

/// Get a backtrace string
extern BASEAPI cstr Plat_GetBackTrace();

#if defined CH_WIN32
/// Get the last Win32 error
#define LastNtError() NtCurrentTeb()->LastErrorValue

/// Get the last NTSTATUS
#define LastNtStatus() NtCurrentTeb()->LastStatusValue

/// Get whether the engine is running in Wine (if you use this for shady stuff you suck)
extern BASEAPI bool Plat_IsWine();

#ifdef CH_UWP
/// Get whether the engine is running as a UWP app or not
extern BASEAPI bool Plat_IsUwpApp();

// TODO: similar functions for input and etc
#if defined IN_BASE || defined IN_VIDEO

struct UwpVideoInfo
{
	winrt_min::ICoreWindow* window;
	winrt_min::ICoreDispatcher* dispatcher;
	winrt_min::ICoreApplicationView* view;
};

struct UwpVideoCallbacks
{
	void* user;

	bool (*OnSizeChanged)(f32 width, f32 height, void* user);
	bool (*OnClosed)(void* user);
};

/// Set up the video system to be able to access the app
extern BASEAPI void Plat_BindUwpVideo(UwpVideoInfo& info, const UwpVideoCallbacks& callbacks);
#endif
#endif

/// Declare the _Available function for an imported function
#define DECLARE_AVAILABLE(func) extern "C" BASEAPI bool func##_Available()

/// Major version of running NT
#define NT_MAJOR_VERSION (USER_SHARED_DATA->NtMajorVersion)

/// Minor version of running NT
#define NT_MINOR_VERSION (USER_SHARED_DATA->NtMinorVersion)

/// Build number of running NT
#define NT_BUILD_NUMBER (USER_SHARED_DATA->NtBuildNumber)

/// Get whether the OS is Windows XP or above
#define AT_LEAST_WINDOWS_XP() (NT_MAJOR_VERSION > 5 || (NT_MAJOR_VERSION == 5 && NT_MINOR_VERSION >= 1))

/// Get whether the OS is Windows Vista or above
#define AT_LEAST_WINDOWS_VISTA() (NT_MAJOR_VERSION >= 6)

/// Get whether the OS is Windows 7 or above
#define AT_LEAST_WINDOWS_7()                                                                                                     \
	(NT_MAJOR_VERSION > 6 || (NT_MAJOR_VERSION == 6 && NT_MINOR_VERSION >= 1))

/// Get whether the OS is Windows 10 or above
#define AT_LEAST_WINDOWS_10() (NT_MAJOR_VERSION >= 10)

/// Get whether ths OS is at least a specific build of windows 10 or greater
#define AT_LEAST_WINDOWS_10_BUILD(build) (NT_MAJOR_VERSION > 10 || (NT_MAJOR_VERSION == 10 && NT_BUILD_NUMBER >= (build)))

/// Get whether the OS is Windows 11 or above
#define AT_LEAST_WINDOWS_11() AT_LEAST_WINDOWS_10_BUILD(22000)
#endif
