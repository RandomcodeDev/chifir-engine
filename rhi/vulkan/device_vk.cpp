#include "device_vk.h"
#include "base/log.h"
#include "vulkan.h"

CVulkanRhiDevice::CVulkanRhiDevice(const VulkanDeviceInfo_t& info) : m_info(info)
{
}

bool CVulkanRhiDevice::Initialize()
{
	

	return true;
}

static void ConvertDeviceInfo(
	RhiDeviceInfo_t& info, ssize index, const VkPhysicalDeviceProperties& properties,
	const VkPhysicalDeviceMemoryProperties& memoryProperties)
{
	static const RhiDeviceType_t DEVICE_TYPES[] = {
		RhiDeviceType_t::Other, RhiDeviceType_t::Integrated, RhiDeviceType_t::Discrete, RhiDeviceType_t::Other,
		RhiDeviceType_t::Software};

	info.name = properties.deviceName;

	info.deviceType = DEVICE_TYPES[properties.deviceType];

	info.vendorId = properties.vendorID;
	info.deviceId = properties.deviceID;

	info.maxTextureSize = properties.limits.maxImageDimension2D;
	info.totalMemory = memoryProperties.memoryHeaps[0].size;

	info.handle = index;
}

bool CVulkanRhiDevice::GetDeviceInfo(
	RhiDeviceInfo_t& rhiInfo, VulkanDeviceInfo_t& info, VkPhysicalDevice device, VkSurfaceKHR surface, ssize i)
{
	Log_Debug("Getting information for device %zd", i);

	VkPhysicalDeviceProperties properties = {};
	vkGetPhysicalDeviceProperties(device, &properties);

	VkPhysicalDeviceMemoryProperties memoryProperties = {};
	vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

	ConvertDeviceInfo(rhiInfo, i, properties, memoryProperties);
	info.device = device;

	u32 queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
	if (queueCount < 1)
	{
		Log_Error("Device %zd is unusable, it has no queue families", i);
		return false;
	}

	info.queueFamilyProperties.Resize(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, info.queueFamilyProperties.Data());

	info.graphicsFamilyIndex = UINT32_MAX;
	info.presentFamilyIndex = UINT32_MAX;
	for (ssize j = 0; j < info.queueFamilyProperties.Size(); j++)
	{
		VkQueueFamilyProperties* properties = &info.queueFamilyProperties[j];
		if (properties->queueCount < 1)
		{
			continue;
		}

		if (properties->queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			info.graphicsFamilyIndex = static_cast<u32>(j);
		}

		VkBool32 presentSupported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, static_cast<u32>(j), surface, &presentSupported);
		if (presentSupported)
		{
			info.presentFamilyIndex = static_cast<u32>(j);
		}

		if (info.graphicsFamilyIndex < UINT32_MAX && info.presentFamilyIndex < UINT32_MAX)
		{
			break;
		}
	}

	if (info.graphicsFamilyIndex == UINT32_MAX || info.presentFamilyIndex == UINT32_MAX)
	{
		Log_Error(
			"Device %zd is unusable, couldn't find required queue families (graphics %u, present %u)", device,
			info.graphicsFamilyIndex, info.presentFamilyIndex);
		return false;
	}

	u32 extensionCount = 0;
	VkResult result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get device extension properties for device %zd: %s", i, GetVkResultString(result));
		return false;
	}

	if (extensionCount < ArraySize(REQUIRED_EXTENSIONS))
	{
		Log_Warning(
			"Device %zd is unusable, it has %u extensions when there are %u required", i, extensionCount,
			ArraySize<u32>(REQUIRED_EXTENSIONS));
		return false;
	}

	info.extensionProperties.Resize(extensionCount);
	result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, info.extensionProperties.Data());
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get device extension properties for device %zd: %s", i, GetVkResultString(result));
		return false;
	}

	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &info.surfaceCapabilities);
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get surface capabilities for device %zd: %s", i, GetVkResultString(result));
		return false;
	}

	u32 formatCount = 0;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get surface formats for device %zd: %s", i, GetVkResultString(result));
		return false;
	}
	if (formatCount < 1)
	{
		Log_Error("Device %zd doesn't have any surface formats", i);
		return false;
	}

	info.surfaceFormats.Resize(formatCount);
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, info.surfaceFormats.Data());
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get surface formats for device %zd: %s", i, GetVkResultString(result));
		return false;
	}

	u32 modeCount = 0;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, 0);
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get present modes for device %zd: %s", i, GetVkResultString(result));
		return false;
	}
	if (modeCount < 1)
	{
		Log_Error("Device %zd doesn't have any present modes", i);
		return false;
	}

	info.presentModes.Resize(modeCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, info.presentModes.Data());
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get present modes for device %zd: %s", i, GetVkResultString(result));
		return false;
	}

	return true;
}
