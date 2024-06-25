// Platform API

#pragma once

#ifdef KR_WIN32
#define PHNT_MODE    PHNT_MODE_USER
#define PHNT_VERSION PHNT_WIN11
// phnt.h depends on phnt_windows.h
// clang-format: off
#include "phnt_windows.h"
#include "phnt.h"
// clang-format: on
#endif

#include "base.h"
#include "compiler.h"
#include "dll.h"
#include "types.h"

// Initialize anything necessary to use platform functions. Terminates the process on failure.
extern BASEAPI void Plat_Init();

// Shut down platform
extern BASEAPI void Plat_Shutdown();
