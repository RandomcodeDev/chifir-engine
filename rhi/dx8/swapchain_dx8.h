/// \file DirectX 8 swap chain implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/vector.h"

#include "rhi/irhiimage.h"
#include "rhi/irhiswapchain.h"

#include "dx8.h"

class CDx8RhiDevice;

class CDx8RhiSwapChain: public IRhiSwapChain, CDx8RhiBaseObject<void>
{
  public:
	virtual void Destroy();
	virtual void GetBuffers(CVector<IRhiRenderTarget*>& images);
	virtual void ResizeBuffers();
	virtual u32 GetFrameIndex()
	{
		return 0; // managed by d3d8
	}
	virtual u32 Present();

  private:
	friend class CDx8RhiDevice;

	CVector<IRhiRenderTarget*> m_buffers;
    ssize m_bufferCount;

	CDx8RhiSwapChain(CDx8RhiDevice* device);
	bool Initialize(u32 bufferCount);
};
