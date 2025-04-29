/// \file Vulkan instance implementation
/// \copyright Randomcode Developers

#pragma once

#include "base/vector.h"

#include "rhi/irhidevice.h"
#include "rhi/irhiinstance.h"

#include "device_vk.h"
#include "vulkan.h"

class ILibrary;
class IVideoSystem;

class CVulkanRhiInstance: public IRhiInstance
{
  public:
	CVulkanRhiInstance() : m_vulkanLib(nullptr), m_instance(VK_NULL_HANDLE), m_debugMessenger(VK_NULL_HANDLE), m_surface(VK_NULL_HANDLE)
	{
	}
	~CVulkanRhiInstance() DEFAULT;

	virtual bool Initialize(IVideoSystem* videoSystem);
	virtual void Destroy();

	virtual void GetDeviceInfo(CVector<RhiDeviceInfo_t>& info);

	virtual IRhiDevice* CreateDevice(const RhiDeviceInfo_t& info);

  private:
	ILibrary* m_vulkanLib;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkSurfaceKHR m_surface;

	CVector<VulkanDeviceInfo_t> m_devices;

	bool CreateSurface(IVideoSystem* videoSystem);
};
