// Platform API

#pragma once

#ifdef CH_WIN32
#include "phnt_wrapper.h"
#include "win32_aliases.h"

// Get the last Win32 error
#define LastNtError() NtCurrentTeb()->LastErrorValue

// Get the last NTSTATUS
#define LastNtStatus() NtCurrentTeb()->LastStatusValue

// Declare the _Available function for an imported function
#define DECLARE_AVAILABLE(func) extern "C" BASEAPI bool func##_Available()
#endif

#include "base.h"
#include "compiler.h"
#include "dll.h"
#include "types.h"

// Initialize anything necessary to use platform functions. Terminates the process on failure.
extern BASEAPI void Plat_Init();

// Shut down platform
extern BASEAPI void Plat_Shutdown();

// Get a description of the OS
extern BASEAPI cstr Plat_GetSystemDescription();

// Get a description of the hardware (CPU, RAM)
extern BASEAPI cstr Plat_GetHardwareDescription();

// Write to the terminal/console (stdout)
extern BASEAPI void Plat_WriteConsole(cstr message);

// Get a timestamp in milliseconds (the point it's relative too depends on the platform and isn't reliable)
extern BASEAPI u64 Plat_GetMilliseconds();
