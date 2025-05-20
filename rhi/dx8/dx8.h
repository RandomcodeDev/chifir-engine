/// \file DirectX 8 definitions
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/platform.h"

#include "rhi/irhibuffer.h"
#include "rhi/irhiimage.h"
#ifdef CH_DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d8.h>
#include <d3d8types.h>

#include "base/meta.h"

#include "rhi/rhi.h"

class CDx8RhiDevice;

template <typename H, typename P = CDx8RhiDevice> class CDx8RhiBaseObject: public IRhiBaseObject
{
  public:
	CDx8RhiBaseObject() = default;
	CDx8RhiBaseObject(P* parent, H* handle = nullptr) : m_parent(parent), m_handle(handle)
	{
	}
	virtual ~CDx8RhiBaseObject() = default;

	virtual void Destroy()
	{
		if constexpr (IsBaseOf<IUnknown, H>())
		{
			if (m_handle)
			{
				m_handle->Release();
			}
		}
	}

	virtual H* GetHandle()
	{
		return m_handle;
	}

  protected:
	P* m_parent;
	H* m_handle;
};

FORCEINLINE u32 RgbaToArgb(u32 rgba)
{
	// rotate a from end to start
	return ROTR32(rgba, 8);
}

extern D3DPOOL GetPoolForMemoryLocation(RhiMemoryLocation location);
extern DWORD GetUsageForImageUsage(RhiImageUsage usage);
extern D3DFORMAT GetDx8Format(RhiImageFormat format);
extern RhiImageFormat GetRhiFormat(D3DFORMAT format);
