/// \file Vulkan device implementation
/// \copyright 2025 Randomcode Developers

#include "base/log.h"

#include "rhi/irhidevice.h"

#include "device_vk.h"
#include "swapchain_vk.h"
#include "vulkan.h"

/// Required device extensions
constexpr cstr REQUIRED_EXTENSIONS[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

CVulkanRhiDevice::CVulkanRhiDevice(CVulkanRhiInstance* instance, const VulkanDeviceInfo_t& info)
	: m_instance(instance), m_info(info)
{
}

bool CVulkanRhiDevice::Initialize()
{
	VkDeviceCreateInfo deviceCreateInfo = {};
	VkDeviceQueueCreateInfo queueCreateInfos[2] = {};
	f32 queuePriority = 1.0f;

	Log_Debug("Creating VkDevice");

	queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[0].queueFamilyIndex = m_info.graphicsFamilyIndex;
	queueCreateInfos[0].pQueuePriorities = &queuePriority;
	queueCreateInfos[0].queueCount = 1;
	deviceCreateInfo.queueCreateInfoCount = 1;
	if (m_info.graphicsFamilyIndex != m_info.presentFamilyIndex)
	{
		queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[1].queueFamilyIndex = m_info.presentFamilyIndex;
		queueCreateInfos[1].pQueuePriorities = &queuePriority;
		queueCreateInfos[1].queueCount = 1;
		deviceCreateInfo.queueCreateInfoCount = 2;
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = true;

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.ppEnabledExtensionNames = REQUIRED_EXTENSIONS;
	deviceCreateInfo.enabledExtensionCount = ArraySize<u32>(REQUIRED_EXTENSIONS);

	VkResult result = vkCreateDevice(m_info.device, &deviceCreateInfo, GetVkAllocationCallbacks(), &m_handle);
	if (result != VK_SUCCESS)
	{
		Log_Error("Failed to create VkDevice: %s", GetVkResultString(result));
		return false;
	}

	volkLoadDevice(m_handle);

	Log_Debug("Getting queues");
	vkGetDeviceQueue(m_handle, m_info.graphicsFamilyIndex, 0, &m_graphicsQueue);
	NameVkObject(m_handle, m_graphicsQueue, VK_OBJECT_TYPE_QUEUE, "Graphics queue");
	vkGetDeviceQueue(m_handle, m_info.presentFamilyIndex, 0, &m_presentQueue);
	NameVkObject(m_handle, m_graphicsQueue, VK_OBJECT_TYPE_QUEUE, "Present queue");

	volkLoadDevice(m_handle);

	return true;
}

void CVulkanRhiDevice::Destroy()
{
	if (m_handle)
	{
		Log_Debug("Destroying VkDevice 0x%016zX", m_handle);
		vkDestroyDevice(m_handle, GetVkAllocationCallbacks());
		m_handle = VK_NULL_HANDLE;
	}
}

IRhiSwapChain* CVulkanRhiDevice::CreateSwapChain(u32 bufferCount)
{
	CVulkanRhiSwapChain* swapChain = new CVulkanRhiSwapChain(this);
	if (!swapChain->Initialize(bufferCount))
	{
		delete swapChain;
		return nullptr;
	}

	return swapChain;
}

static void ConvertDeviceInfo(
	RhiDeviceInfo_t& info, ssize index, const VkPhysicalDeviceProperties& properties,
	const VkPhysicalDeviceMemoryProperties& memoryProperties)
{
	static const RhiDeviceType DEVICE_TYPES[] = {
		RhiDeviceType::Other, RhiDeviceType::Integrated, RhiDeviceType::Discrete, RhiDeviceType::Other,
		RhiDeviceType::Software};

	info.name = properties.deviceName;

	info.deviceType = DEVICE_TYPES[properties.deviceType];

	info.vendorId = properties.vendorID;
	info.deviceId = properties.deviceID;

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

	if (extensionCount < ArraySize<u32>(REQUIRED_EXTENSIONS))
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
