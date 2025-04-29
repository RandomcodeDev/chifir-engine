/// \file SDL video system interface
/// \copyright Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

class ISdlVideoSystem
{
  public:
    virtual ~ISdlVideoSystem() = default;

    /// Create a Vulkan surface
    virtual u64 CreateVulkanSurface(u64 instance, const void* allocCallbacks) = 0;
};
