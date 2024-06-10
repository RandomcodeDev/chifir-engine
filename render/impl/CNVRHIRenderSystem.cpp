#include "CDX12NVRHIDeviceManager.h"
#include "CNVRHIRenderSystem.h"

bool CNVRHIRenderSystem::Initialize(IVideoSystem* videoSystem)
{
	KR_LOG_INFO("Initializing NVRHI render system");

	m_videoSystem = videoSystem;

#ifdef KR_DX12
	m_deviceMgr = std::make_unique<CDX12NVRHIDeviceManager>();
#else
	KR_LOG_ERROR("No render system implementation!");
	return false;
#endif

	m_device = m_deviceMgr->CreateDevice();
	if (!m_device)
	{
		KR_LOG_ERROR("Failed to create NVRHI device!");
		return false;
	}

	switch (m_device->getGraphicsAPI())
	{
	case nvrhi::GraphicsAPI::D3D11:
		m_apiName = "Direct3D 11";
		break;
	case nvrhi::GraphicsAPI::D3D12:
		m_apiName = "Direct3D 12";
		break;
	case nvrhi::GraphicsAPI::VULKAN:
		m_apiName = "Vulkan";
		break;
	default:
		m_apiName = "unknown";
		break;
	}

	if (!m_deviceMgr->CreateSwapChain(m_videoSystem))
	{
		return false;
	}

	KR_LOG_INFO("NVRHI render system using {} initialized", m_apiName);

	return true;
}

void CNVRHIRenderSystem::WaitForGpu()
{
	m_device->waitForIdle();
}

void CNVRHIRenderSystem::Shutdown()
{
	KR_LOG_INFO("Shutting down NVRHI render system");

	WaitForGpu();

	m_device = nullptr;
	m_deviceMgr = nullptr;

	KR_LOG_INFO("NVRHI render system shut down");
}

void CNVRHIRenderSystem::BeginFrame()
{
	m_deviceMgr->BeginFrame(m_videoSystem);
}

void CNVRHIRenderSystem::EndFrame()
{
	WaitForGpu();
	m_device->runGarbageCollection();

	m_deviceMgr->Present();
}

void CNVRHIRenderSystem::ClearColour(rtm::vector4d colour)
{
}

bool CNVRHIRenderSystem::CreateCommandLists()
{
	return false;
}
