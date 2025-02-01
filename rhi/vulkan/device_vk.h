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
  private:
	friend class CVulkanRhiInstance;

	static bool GetDeviceInfo(RhiDeviceInfo_t& rhiInfo, VulkanDeviceInfo_t& info, VkPhysicalDevice device, VkSurfaceKHR surface, ssize i);

	static constexpr cstr REQUIRED_EXTENSIONS[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
