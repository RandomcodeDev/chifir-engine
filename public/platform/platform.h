// Platform API

#pragma once

#ifdef KR_WIN32
#define PHNT_MODE PHNT_MODE_USER
#define PHNT_VERSION PHNT_WIN11
#include "phnt_windows.h"
#include "phnt.h"
#endif

#include "base/dll.h"

#ifdef IN_PLATFORM
#define DLLAPI DLLEXPORT
#else
#define PLATAPI DLLIMPORT
#endif

// Initialize anything necessary to use platform functions. Terminates the process on failure.
extern PLATAPI void Plat_Init();

// Shut down platform
extern PLATAPI void Plat_Shutdown();
