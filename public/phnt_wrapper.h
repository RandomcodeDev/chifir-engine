// Fixes phnt

#pragma once

#define PHNT_MODE PHNT_MODE_USER
#ifdef CH_XBOX360
#define PHNT_VERSION PHNT_WIN2K
#include "phnt_xbox.h"
#else
#define PHNT_VERSION PHNT_WIN11
#define ETW_WOW6432
// ntwmi.h doesn't work without this
#include "phnt_windows.h"
#endif
// These are publicly declared in the 360 SDK, so phnt's definitions aren't allowed
#ifdef CH_XBOX360
#define RtlLockHeap                              RtlLockHeap_phnt
#define RtlInitializeCriticalSection             RtlInitializeCriticalSection_phnt
#define RtlInitializeCriticalSectionAndSpinCount RtlInitializeCriticalSectionAndSpinCount_phnt
#define RtlEnterCriticalSection                  RtlEnterCriticalSection_phnt
#define RtlLeaveCriticalSection                  RtlLeaveCriticalSection_phnt
#endif
#include <evntrace.h>
#include "phnt.h"
#ifdef CH_XBOX360
#undef RtlLockHeap
#undef RtlInitializeCriticalSection
#undef RtlInitializeCriticalSectionAndSpinCount
#undef RtlEnterCriticalSection
#undef RtlLeaveCriticalSection
#endif
