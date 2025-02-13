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
};

class CVulkanRhiDevice: public IRhiDevice
{
  public:
	virtual IRhiCommandQueue* CreateCommandQueue(RhiCommandQueueType_t type, RhiCommandQueueFlags_t flags)
	{
		(void)type;
		(void)flags;
		return nullptr;
	}
	virtual IRhiCommandList* CreateCommandList(IRhiCommandQueue* queue, RhiCommandListFlags_t flags)
	{
		(void)queue;
		(void)flags;
		return nullptr;
	}
	virtual IRhiFence* CreateFence(u64 initialValue = 0)
	{
		(void)initialValue;
		return nullptr;
	}
	virtual IRhiBuffer* CreateBuffer(ssize size, RhiMemoryLocation_t location, RhiMemoryUsage_t usage, RhiBufferFlags_t flags)
	{
		(void)size;
		(void)location;
		(void)usage;
		(void)flags;
		return nullptr;
	}
	virtual IRhiImage* CreateImage(
		u32 width, u32 height, u32 depth, RhiMemoryLocation_t location, RhiImageType_t type, RhiImageFormat_t format,
		RhiImageFlags_t flags)
	{
		(void)width;
		(void)height;
		(void)location;
		(void)type;
		(void)format;
		(void)flags;
		return nullptr;
	}
	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc)
	{
		(void)desc;
		return nullptr;
	}

  private:
	friend class CVulkanRhiInstance;

	VulkanDeviceInfo_t m_info;
	VkDevice m_handle;

	CVulkanRhiDevice(const VulkanDeviceInfo_t& info);
	bool Initialize();

	/// Get information about a device. Returns false if the device isn't usable.
	static bool GetDeviceInfo(
		RhiDeviceInfo_t& rhiInfo, VulkanDeviceInfo_t& info, VkPhysicalDevice device, VkSurfaceKHR surface, ssize i);

	/// Required device extensions
	static constexpr cstr REQUIRED_EXTENSIONS[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
