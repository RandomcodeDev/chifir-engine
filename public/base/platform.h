/// Platform API

#pragma once

#ifdef CH_WIN32
#include "phnt_wrapper.h"
#include "win32_aliases.h"

/// Get the last Win32 error
#define LastNtError() NtCurrentTeb()->LastErrorValue

/// Get the last NTSTATUS
#define LastNtStatus() NtCurrentTeb()->LastStatusValue

/// Declare the _Available function for an imported function
#define DECLARE_AVAILABLE(func) extern "C" BASEAPI bool func##_Available()
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

/// Pause the thread for the given amount of time
extern BASEAPI void Plat_Sleep(u64 milliseconds);

/// Get the date and time
extern BASEAPI void Plat_GetDateTime(DateTime_t& time, bool utc = false);

/// Get the location for saving stuff
extern BASEAPI cstr Plat_GetSaveLocation();

/// Get an environment variable
extern BASEAPI cstr Plat_GetEnvironment(cstr name);
