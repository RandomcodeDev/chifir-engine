#pragma once

#include "base/compiler.h"
#include "base/types.h"

#ifdef IN_RHI
#define RHIAPI DLLEXPORT
#else
#define RHIAPI DLLIMPORT
#endif

enum class RhiBackendType_t : s32
{
	None,
	Vulkan,
	Unknown,
};
