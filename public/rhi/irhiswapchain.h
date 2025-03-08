#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "base/vector.h"

#include "irhiimage.h"
#include "irhisemaphore.h"
#include "rhi.h"

/// Represents an IDXGISwapChain3, a VkSwapchainKHR, or similar
class IRhiSwapChain
{
  public:
    virtual ~IRhiSwapChain() DEFAULT;

    /// Destroy the swap chain
    virtual void Destroy() = 0;

    /// Resize backbuffers to match the window size, invalidates previous images from this swapchain
    virtual void ResizeBuffers(CVector<IRhiRenderTarget*>& buffers) = 0;

    /// Present, waiting for rendering to complete, returning the new current backbuffer index
    virtual u32 Present(IRhiSemaphore* renderCompletedSemaphore) = 0;
};
