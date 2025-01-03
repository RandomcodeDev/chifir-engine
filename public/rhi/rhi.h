#pragma once

#include "base/compiler.h"

#ifdef IN_RHI
#define RHIAPI DLLEXPORT
#else
#define RHIAPI DLLIMPORT
#endif

enum RhiBackendType_t
{
	RhiBackendTypeNone,
	RhiBackendTypeVulkan,
	RhiBackendTypeUnknown,
};
