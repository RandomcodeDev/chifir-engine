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
	case RhiBackendType_t::Vulkan: {
#ifdef RHI_VULKAN
		return Vulkan_CreateInstance();
#else
		Log_Error("Vulkan support not in available in this build!");
#endif
	}
	case RhiBackendType_t::None:
	case RhiBackendType_t::Unknown:
	default:
		return nullptr;
	}
}
