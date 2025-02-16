#pragma once

#include "base/vector.h"
#include "rhi/irhidevice.h"
#include "rhi/rhi.h"

#include "vulkan.h"

struct VulkanDeviceInfo_t
{
	VkPhysicalDevice device;

	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceMemoryProperties memoryProperties;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	CVector<VkSurfaceFormatKHR> surfaceFormats;

	CVector<VkPresentModeKHR> presentModes;

	CVector<VkQueueFamilyProperties> queueFamilyProperties;
	u32 graphicsFamilyIndex;
	u32 presentFamilyIndex;

	CVector<VkExtensionProperties> extensionProperties;

	VulkanDeviceInfo_t() DEFAULT;
};

class CVulkanRhiDevice: public IRhiDevice
{
  public:
	virtual void Destroy();

	virtual IRhiCommandQueue* CreateCommandQueue(RhiCommandQueueType_t type, RhiCommandQueueFlags_t flags)
	{
		UNUSED(type);
		UNUSED(flags);
		return nullptr;
	}
	virtual IRhiCommandList* CreateCommandList(IRhiCommandQueue* queue, RhiCommandListFlags_t flags)
	{
		UNUSED(queue);
		UNUSED(flags);
		return nullptr;
	}
	virtual IRhiFence* CreateFence(u64 initialValue = 0)
	{
		UNUSED(initialValue);
		return nullptr;
	}
	virtual IRhiBuffer* CreateBuffer(ssize size, RhiMemoryLocation_t location, RhiMemoryUsage_t usage, RhiBufferFlags_t flags)
	{
		UNUSED(size);
		UNUSED(location);
		UNUSED(usage);
		UNUSED(flags);
		return nullptr;
	}
	virtual IRhiImage* CreateImage(
		u32 width, u32 height, u32 depth, RhiMemoryLocation_t location, RhiImageType_t type, RhiImageFormat_t format,
		RhiImageFlags_t flags)
	{
		UNUSED(width);
		UNUSED(height);
		UNUSED(location);
		UNUSED(type);
		UNUSED(format);
		UNUSED(flags);
		return nullptr;
	}
	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc)
	{
		UNUSED(desc);
		return nullptr;
	}

  private:
	friend class CVulkanRhiInstance;

	VulkanDeviceInfo_t m_info;
	VkDevice m_handle;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	CVulkanRhiDevice(const VulkanDeviceInfo_t& info);
	bool Initialize();

	/// Get information about a device. Returns false if the device isn't usable.
	static bool GetDeviceInfo(
		RhiDeviceInfo_t& rhiInfo, VulkanDeviceInfo_t& info, VkPhysicalDevice device, VkSurfaceKHR surface, ssize i);
};
