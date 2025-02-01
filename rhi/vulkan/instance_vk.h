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
	CVulkanRhiInstance() : m_instance(VK_NULL_HANDLE)
	{
	}
	~CVulkanRhiInstance() DEFAULT;

	bool Initialize(IVideoSystem* videoSystem);
	void Destroy();

	void GetDeviceInfo(CVector<RhiDeviceInfo_t>& info);

	IRhiDevice* CreateDevice(s32 index)
	{
		(void)index;
		return nullptr;
	}

	IRhiSwapChain* CreateSwapChain()
	{
		return nullptr;
	}

  private:
	ILibrary* m_vulkanLib;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkSurfaceKHR m_surface;

	CVector<VulkanDeviceInfo_t> m_devices;
};

extern RHIAPI IRhiInstance* Vulkan_CreateInstance();
