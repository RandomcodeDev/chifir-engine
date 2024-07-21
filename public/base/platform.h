// Platform API

#pragma once

#ifdef CH_WIN32
#include "phnt_wrapper.h"

// Get the last Win32 error
#define LastNtError() NtCurrentTeb()->LastErrorValue

// Get the last NTSTATUS
#define LastNtStatus() NtCurrentTeb()->LastStatusValue
#endif

#include "base.h"
#include "compiler.h"
#include "dll.h"
#include "types.h"

// Initialize anything necessary to use platform functions. Terminates the process on failure.
BASEAPI void Plat_Init();

// Shut down platform
BASEAPI void Plat_Shutdown();
