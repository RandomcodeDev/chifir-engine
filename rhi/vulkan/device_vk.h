/// \file Vulkan device implementation
/// \copyright 2025 Randomcode Developers

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

	VulkanDeviceInfo_t() = default;
};

class CVulkanRhiInstance;

class CVulkanRhiDevice: public IRhiDevice
{
  public:
	virtual void Destroy();

	virtual IRhiSwapChain* CreateSwapChain(u32 bufferCount);

	virtual IRhiCommandList* CreateCommandList(RhiCommandListFlags flags, ssize bufferSize)
	{
		UNUSED(flags);
		return nullptr;
	}
	virtual IRhiFence* CreateFence(u64 initialValue = 0)
	{
		UNUSED(initialValue);
		return nullptr;
	}
	virtual IRhiBuffer* CreateBuffer(ssize size, RhiMemoryLocation location, RhiBufferUsage usage)
	{
		UNUSED(size);
		UNUSED(location);
		UNUSED(usage);
		return nullptr;
	}
	virtual IRhiImage* CreateImage2d(
		u32 width, u32 height, u32 depth, RhiMemoryLocation location, RhiImageType type, RhiImageFormat format,
		RhiImageUsage usage)
	{
		UNUSED(width);
		UNUSED(height);
		UNUSED(depth);
		UNUSED(location);
		UNUSED(type);
		UNUSED(format);
		return nullptr;
	}
	virtual IRhiImageView* CreateImageView(IRhiImage* image)
	{
		UNUSED(image);
		return nullptr;
	}
	virtual IRhiRenderTarget* CreateRenderTarget(IRhiImageView* imageView)
	{
		UNUSED(imageView);
		return nullptr;
	}
	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc)
	{
		UNUSED(desc);
		return nullptr;
	}

	virtual void ExecuteCommandList(IRhiCommandList* cmdList)
	{
		ExecuteCommandLists(&cmdList, 1);
	}

	virtual void ExecuteCommandLists(IRhiCommandList** cmdLists, ssize count)
	{
		UNUSED(cmdLists);
		UNUSED(count);
	}

  private:
	friend class CVulkanRhiInstance;

	CVulkanRhiInstance* m_instance;
	VulkanDeviceInfo_t m_info;
	VkDevice m_handle;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	CVulkanRhiDevice(CVulkanRhiInstance* instance, const VulkanDeviceInfo_t& info);
	bool Initialize();

	/// Get information about a device. Returns false if the device isn't usable.
	static bool GetDeviceInfo(
		RhiDeviceInfo_t& rhiInfo, VulkanDeviceInfo_t& info, VkPhysicalDevice device, VkSurfaceKHR surface, ssize i);
};
