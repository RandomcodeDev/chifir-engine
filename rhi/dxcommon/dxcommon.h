/// \file Common legacy DirectX utilities
/// \copyright 2025 Randomcode Developers

#pragma once

#ifdef CH_DEBUG
#define D3D_DEBUG_INFO
#endif

#include "base/meta.h"

#include "rhi/irhiimage.h"
#include "rhi/irhimemory.h"

#include "dxver.h"

template <typename H, typename P = CDxRhiDevice> class CDxRhiBaseObject: public IRhiBaseObject
{
  public:
	CDxRhiBaseObject() = default;
	CDxRhiBaseObject(P* parent, H* handle = nullptr) : m_parent(parent), m_handle(handle)
	{
	}
	virtual ~CDxRhiBaseObject() = default;

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

#if defined IN_RHI_DIRECTX8 || defined IN_RHI_DIRECTX9
static FORCEINLINE u32 RgbaToArgb(u32 rgba)
{
	// rotate a from end to start
	return ROTR32(rgba, 8);
}

extern D3DPOOL GetPoolForMemoryLocation(RhiMemoryLocation location);
extern DWORD GetUsageForImageUsage(RhiImageUsage usage);
extern D3DFORMAT GetDxFormat(RhiImageFormat format);
extern RhiImageFormat GetRhiFormat(D3DFORMAT format);
#endif
