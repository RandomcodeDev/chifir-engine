#pragma once

#include "base/compiler.h"
#include "base/types.h"

#ifdef CH_STATIC
#define RHIAPI
#else
#ifdef IN_RHI
#define RHIAPI DLLEXPORT
#else
#define RHIAPI DLLIMPORT
#endif
#endif

enum class RhiBackendType_t : s32
{
	None,
	Vulkan,
	DirectX12,
	DirectX9,
	OpenGl,
	Unknown,
};
