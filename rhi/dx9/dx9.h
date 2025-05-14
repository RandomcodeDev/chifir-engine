/// \file DirectX 9 definitions
/// \copyright 2025 Randomcode Developers

#pragma once

#ifdef CH_DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3d9types.h>

#include "base/meta.h"

#include "rhi/rhi.h"

class CDx9RhiDevice;

template <typename H, typename P = CDx9RhiDevice> class CDx9RhiBaseObject: public IRhiBaseObject
{
  public:
	CDx9RhiBaseObject() = default;
	CDx9RhiBaseObject(P* parent, H* handle = nullptr) : m_parent(parent), m_handle(handle)
	{
	}
	virtual ~CDx9RhiBaseObject() = default;

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
