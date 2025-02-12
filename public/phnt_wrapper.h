// Fixes phnt

#pragma once

#define PHNT_MODE PHNT_MODE_USER
#ifdef CH_XBOX360
#define PHNT_VERSION PHNT_WIN2K
// _Very_ questionable wrapper that I made by copying definitions that probably don't quite work
#include "phnt_xbox.h"
#else
#define PHNT_VERSION PHNT_WIN11
#include "phnt_windows.h"
#include <evntrace.h>
#endif
// These are publicly declared in the 360 SDK, so phnt's definitions aren't allowed
#ifdef CH_XBOX360
#define RtlLockHeap                              RtlLockHeap_phnt
#define RtlUnlockHeap                            RtlUnlockHeap_phnt
#define RtlAllocateHeap                          RtlAllocateHeap_phnt
#define RtlFreeHeap                              RtlFreeHeap_phnt
#define RtlReAllocateHeap                        RtlReAllocateHeap_phnt
#define RtlCompactHeap                           RtlCompactHeap_phnt
#define RtlSizeHeap                              RtlSizeHeap_phnt
#define RtlValidateHeap                          RtlValidateHeap_phnt
#define RtlDeleteCriticalSection                 RtlDeleteCriticalSection_phnt
#define RtlInitializeCriticalSection             RtlInitializeCriticalSection_phnt
#define RtlInitializeCriticalSectionAndSpinCount RtlInitializeCriticalSectionAndSpinCount_phnt
#define RtlEnterCriticalSection                  RtlEnterCriticalSection_phnt
#define RtlTryEnterCriticalSection               RtlTryEnterCriticalSection_phnt
#define RtlLeaveCriticalSection                  RtlLeaveCriticalSection_phnt
#define ExitProcess(...)
#endif
#include "phnt.h"
#ifdef CH_XBOX360
#undef RtlLockHeap
#undef RtlUnlockHeap
#undef RtlAllocateHeap
#undef RtlFreeHeap
#undef RtlReAllocateHeap
#undef RtlCompactHeap
#undef RtlSizeHeap
#undef RtlValidateHeap
#undef RtlDeleteCriticalSection
#undef RtlInitializeCriticalSection
#undef RtlInitializeCriticalSectionAndSpinCount
#undef RtlEnterCriticalSection
#undef RtlTryEnterCriticalSection
#undef RtlLeaveCriticalSection
#undef ExitProcess
#endif

// normal one has an unnecessary (PWCH) cast
#undef RTL_CONSTANT_STRING
#define RTL_CONSTANT_STRING(s) { sizeof((s)) - sizeof((s)[0]), sizeof((s)), (s) }
