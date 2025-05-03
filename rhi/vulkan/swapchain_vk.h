/// \file Vulkan swap chain implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "rhi/irhiimage.h"
#include "rhi/irhiswapchain.h"

class CVulkanRhiDevice;

class CVulkanRhiSwapChain: public IRhiSwapChain
{
  public:
	virtual void Destroy();
	virtual void GetBuffers(CVector<IRhiRenderTarget*>& images);
	virtual void ResizeBuffers();
	virtual u32 GetFrameIndex()
	{
		return m_frameIndex;
	}
	virtual u32 Present();

  private:
	friend class CVulkanRhiDevice;

	CVulkanRhiDevice* m_device;
	u32 m_frameIndex;

	CVulkanRhiSwapChain(CVulkanRhiDevice* device);
	bool Initialize(u32 bufferCount);
};
