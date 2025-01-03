// Exports Rhi_CreateInstance

#include "base/log.h"
#include "rhi/irhiinstance.h"
#include "rhi/rhi.h"

#ifdef RHI_VULKAN
#include "vulkan/instance_vk.h"
#endif

RHIAPI IRhiInstance* Rhi_CreateInstance(RhiBackendType_t type)
{
    switch (type)
    {
    case RhiBackendTypeVulkan:
    {
#ifdef RHI_VULKAN
        CVulkanRhiInstance* instance = new CVulkanRhiInstance();
        return instance;
#else
        Log_Error("Vulkan support not in available in this build!");
#endif
    }
    case RhiBackendTypeNone:
    case RhiBackendTypeUnknown:
    default:
        return nullptr;
    }
}
