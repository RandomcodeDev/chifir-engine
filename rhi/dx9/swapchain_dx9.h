/// \file DirectX 9 swap chain implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/vector.h"

#include "rhi/irhiimage.h"
#include "rhi/irhiswapchain.h"

#include "dx9.h"

class CDx9RhiDevice;

class CDx9RhiSwapChain: public IRhiSwapChain, CDx9RhiBaseObject<IDirect3DSwapChain9>
{
  public:
	virtual void Destroy();
	virtual void GetBuffers(CVector<IRhiRenderTarget*>& images);
	virtual void ResizeBuffers();
	virtual u32 GetFrameIndex()
	{
		return 0; // managed by d3d9
	}
	virtual u32 Present();

  private:
	friend class CDx9RhiDevice;

	CVector<IRhiRenderTarget*> m_buffers;
    u32 m_bufferCount;

	CDx9RhiSwapChain(CDx9RhiDevice* device);
	bool Initialize(u32 bufferCount);
};
