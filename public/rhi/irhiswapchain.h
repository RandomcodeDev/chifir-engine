/// \file RHI swap chain interface
/// \copyright 2025 Randomcode Developers

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

    /// Get the swap chain's buffers
    virtual void GetBuffers(CVector<IRhiRenderTarget*>& buffers) = 0;

    /// Resize backbuffers to match the window size, invalidates previous buffers from this swapchain
    virtual void ResizeBuffers() = 0;

    /// Get the current frame index
    virtual u32 GetFrameIndex() = 0;

    /// Present and get the current frame index
    virtual u32 Present() = 0;
};
