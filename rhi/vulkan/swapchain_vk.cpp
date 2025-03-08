#include "base/log.h"

#include "swapchain_vk.h"

CVulkanRhiSwapChain::CVulkanRhiSwapChain(CVulkanRhiDevice* device) : m_device(device)
{
}

bool CVulkanRhiSwapChain::Initialize()
{
	Log_Debug("Creating VkSwapchainKHR");
	return true;
}

void CVulkanRhiSwapChain::Destroy()
{
}

void CVulkanRhiSwapChain::ResizeBuffers(CVector<IRhiRenderTarget*>& buffers)
{
}

u32 CVulkanRhiSwapChain::Present(IRhiSemaphore* renderCompleteSemaphore)
{
	return 0;
}
