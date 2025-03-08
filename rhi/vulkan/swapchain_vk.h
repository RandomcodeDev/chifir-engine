#pragma once

#include "rhi/irhiimage.h"
#include "rhi/irhiswapchain.h"

class CVulkanRhiSwapChain: public IRhiSwapChain
{
  public:
    virtual void Destroy();
    virtual void ResizeBuffers(CVector<IRhiRenderTarget*>& images);
    virtual u32 Present(IRhiSemaphore* renderCompleteSemaphore);

  private:
	friend class CVulkanRhiDevice;

	CVulkanRhiDevice* m_device;

	CVulkanRhiSwapChain(CVulkanRhiDevice* device);
	bool Initialize();
};
