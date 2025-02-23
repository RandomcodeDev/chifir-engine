#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "base/vector.h"

#include "rhi.h"

class IRhiImage;
class IRhiSemaphore;

// IDXGISwapChain3 / VkSwapchainKHR
class IRhiSwapChain
{
  public:
    virtual ~IRhiSwapChain() DEFAULT;

    // Resize backbuffers, invalidates previous images from this swapchain
    virtual void ResizeBuffers(CVector<IRhiImage*>& buffers);

    // Present, waiting for rendering to complete, returning the new current backbuffer index
    virtual s32 Present(IRhiSemaphore* renderCompletedSemaphore);
};
