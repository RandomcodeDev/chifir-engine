/// \file DirectX 12 definitions
/// \copyright 2025 Randomcode Developers

#pragma once

#include <dxgi1_6.h>
#ifdef CH_DEBUG
#include <dxgidebug.h>
#endif
#include "d3d12.h"

#include "base/meta.h"
#include "base/platform.h"

#include "rhi/rhi.h"

#ifdef CH_SCARLETT
#define DX12_TARGET_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_2
#else
#define DX12_TARGET_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_1
#define DX12_BACKUP_FEATURE_LEVEL D3D_FEATURE_LEVEL_11_0
#endif

class CDx12RhiDevice;

template <typename H, typename P = CDx12RhiDevice> class CDx12RhiBaseObject: public IRhiBaseObject
{
  public:
	CDx12RhiBaseObject() = default;
	CDx12RhiBaseObject(P* parent, H* handle = nullptr) : m_parent(parent), m_handle(handle)
	{
	}
	virtual ~CDx12RhiBaseObject() = default;

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
