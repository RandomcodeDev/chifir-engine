// Platform API

#pragma once

#ifdef CH_WIN32
#define PHNT_MODE PHNT_MODE_USER
#ifdef CH_XBOX360
#define PHNT_VERSION PHNT_WIN2K
#include "phnt_xbox.h"
#else
#define PHNT_VERSION PHNT_WIN11
#include "phnt_windows.h"
#endif
#ifdef CH_XBOX360
#define RtlLockHeap                              RtlLockHeap_phnt
#define RtlInitializeCriticalSection             RtlInitializeCriticalSection_phnt
#define RtlInitializeCriticalSectionAndSpinCount RtlInitializeCriticalSectionAndSpinCount_phnt
#define RtlEnterCriticalSection                  RtlEnterCriticalSection_phnt
#define RtlLeaveCriticalSection                  RtlLeaveCriticalSection_phnt
#endif
#include "phnt.h"
#ifdef CH_XBOX360
#undef RtlLockHeap
#undef RtlInitializeCriticalSection
#undef RtlInitializeCriticalSectionAndSpinCount
#undef RtlEnterCriticalSection
#undef RtlLeaveCriticalSection
#endif
#endif

#include "base.h"
#include "compiler.h"
#include "dll.h"
#include "types.h"

// Initialize anything necessary to use platform functions. Terminates the process on failure.
extern BASEAPI void Plat_Init();

// Shut down platform
extern BASEAPI void Plat_Shutdown();
