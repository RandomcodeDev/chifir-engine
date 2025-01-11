#pragma once

#include "rhi/irhidevice.h"
#include "rhi/irhiinstance.h"
#include "vulkan.h"

class ILibrary;

class RHIAPI CVulkanRhiInstance: public IRhiInstance
{
  public:
	CVulkanRhiInstance() : m_instance(VK_NULL_HANDLE)
	{
	}
	~CVulkanRhiInstance() DEFAULT;

	bool Initialize();
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
};
