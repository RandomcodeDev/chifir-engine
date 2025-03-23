/// \file Vulkan swap chain implementation
/// \copyright Randomcode Developers

#include "base/log.h"

#include "swapchain_vk.h"

CVulkanRhiSwapChain::CVulkanRhiSwapChain(CVulkanRhiDevice* device) : m_device(device)
{
}

bool CVulkanRhiSwapChain::Initialize(u32 bufferCount)
{
	Log_Debug("Creating VkSwapchainKHR");
	return true;
}

void CVulkanRhiSwapChain::Destroy()
{
}

void CVulkanRhiSwapChain::GetBuffers(CVector<IRhiRenderTarget*>& buffers)
{
}

void CVulkanRhiSwapChain::ResizeBuffers()
{
}

u32 CVulkanRhiSwapChain::Present()
{
	return 0;
}
