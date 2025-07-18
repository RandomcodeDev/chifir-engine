/// \file DirectX 12 swap chain implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/vector.h"

#include "rhi/irhiimage.h"
#include "rhi/irhiswapchain.h"

#include "dx12.h"

class CDx12RhiDevice;

class CDx12RhiSwapChain: public IRhiSwapChain, public CDxRhiBaseObject<IDXGISwapChain4, CDx12RhiDevice>
{
  public:
	virtual void Destroy();
	virtual void GetBuffers(CVector<IRhiRenderTarget*>& images);
	virtual void ResizeBuffers();
	virtual u32 GetFrameIndex()
	{
		return m_handle->GetCurrentBackBufferIndex();
	}
	virtual u32 Present();

  private:
	friend class CDx12RhiDevice;

	CVector<IRhiRenderTarget*> m_buffers;
	u32 m_bufferCount;

	CDx12RhiSwapChain(CDx12RhiDevice* device);
	bool Initialize(u32 bufferCount);
};
