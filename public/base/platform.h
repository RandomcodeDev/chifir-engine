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
