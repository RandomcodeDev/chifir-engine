#include "device_vk.h"
#include "base/log.h"

static VulkanDeviceInfo_t* ConvertDeviceInfo(
	RhiDeviceInfo_t& info, VkPhysicalDevice device, const VkPhysicalDeviceProperties& properties,
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

	VulkanDeviceInfo_t* vkInfo = new VulkanDeviceInfo_t;
	vkInfo->device = device;
	vkInfo->properties = properties;
	vkInfo->memoryProperties = memoryProperties;
	info.handle = reinterpret_cast<u64>(vkInfo);

	return vkInfo;
}

bool CVulkanRhiDevice::GetDeviceInfo(RhiDeviceInfo_t& info, VkPhysicalDevice device, VkSurfaceKHR surface, ssize i)
{
	Log_Debug("Getting information for device %zd", i);

	VkPhysicalDeviceProperties properties = {};
	vkGetPhysicalDeviceProperties(device, &properties);

	VkPhysicalDeviceMemoryProperties memoryProperties = {};
	vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

	RhiDeviceInfo_t currentInfo = {};
	VulkanDeviceInfo_t* deviceInfo = ConvertDeviceInfo(currentInfo, device, properties, memoryProperties);
	deviceInfo->device = device;
	currentInfo.freeHandle = true;

	u32 queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
	if (queueCount < 1)
	{
		Log_Error("Device %zd is unusable, it has no queue families", i);
		return false;
	}

	deviceInfo->queueFamilyProperties.Resize(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, deviceInfo->queueFamilyProperties.Data());

	deviceInfo->graphicsFamilyIndex = UINT32_MAX;
	deviceInfo->presentFamilyIndex = UINT32_MAX;
	for (ssize j = 0; j < deviceInfo->queueFamilyProperties.Size(); j++)
	{
		VkQueueFamilyProperties* properties = &deviceInfo->queueFamilyProperties[j];
		if (properties->queueCount < 1)
		{
			continue;
		}

		if (properties->queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			deviceInfo->graphicsFamilyIndex = static_cast<u32>(j);
		}

		Log_Trace("Checking surface support for queue family %zd", j);
		VkBool32 presentSupported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, static_cast<u32>(j), surface, &presentSupported);

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
		Log_Warning("Device %zd is unusable, it has %u extensions when there are %u required", i, extensionCount, ArraySize<u32>(REQUIRED_EXTENSIONS));
		return false;
	}

	deviceInfo->extensionProperties.Resize(extensionCount);

	result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceInfo->extensionProperties.Data());
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to get device extension properties for device %zd: %s", i, GetVkResultString(result));
		return false;
	}

	return true;
}
