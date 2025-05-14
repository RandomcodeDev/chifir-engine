/// \file RHI definitions
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/dll.h"
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

enum class RhiBackendType : s32
{
	None,
	Vulkan,
	DirectX12,
	DirectX9,
	OpenGl,
	Custom,
};

class IRhiBaseObject
{
  public:
	virtual ~IRhiBaseObject() = default;

	virtual void Destroy() = 0;

	//virtual u64 GetHandle() = 0;
};

extern RHIAPI cstr Rhi_GetBackendName(RhiBackendType backend);
extern RHIAPI RhiBackendType Rhi_GetBackendTypeByName(cstr name);
