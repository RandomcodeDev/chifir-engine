#pragma once

#include "rhi/irhiimage.h"
#include "rhi/irhiswapchain.h"

class CVulkanRhiSwapChain: public IRhiSwapChain
{
    virtual void Destroy();
    virtual void ResizeBuffers(CVector<IRhiRenderTarget*>& images) = 0;
    virtual s32 Present(IRhiSemaphore* renderCompleteSemaphore) = 0;
};
