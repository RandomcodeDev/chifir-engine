#include "CDX12NVRHIDeviceManager.h"
#include "CNVRHIRenderSystem.h"

CNVRHIRenderSystem::CNVRHIRenderSystem()
{
}

CNVRHIRenderSystem::~CNVRHIRenderSystem()
{
}

bool CNVRHIRenderSystem::Initialize()
{
#ifdef KR_DX12
	m_deviceMgr = std::make_unique<CDX12NVRHIDeviceManager>();
#else
	KR_LOG_ERROR("No render system implementation!");
	return false;
#endif
	if (!m_deviceMgr)
	{
		KR_LOG_ERROR("Failed to create device manager!");
		return false;
	}

	m_device = m_deviceMgr->CreateDevice();
	if (!m_device)
	{
		KR_LOG_ERROR("Failed to create NVRHI device!");
		return false;
	}

	switch (m_device->getGraphicsAPI())
	{
	default:
		m_apiName = "unknown";
		break;
	case nvrhi::GraphicsAPI::D3D11:
		m_apiName = "Direct3D 11";
		break;
	case nvrhi::GraphicsAPI::D3D12:
		m_apiName = "Direct3D 12";
		break;
	case nvrhi::GraphicsAPI::VULKAN:
		m_apiName = "Vulkan";
		break;
	}

	return true;
}

void CNVRHIRenderSystem::WaitForGpu()
{
}

void CNVRHIRenderSystem::Shutdown()
{
}

void CNVRHIRenderSystem::BeginFrame()
{
}

void CNVRHIRenderSystem::EndFrame()
{
}

void CNVRHIRenderSystem::ClearColour(rtm::vector4d colour)
{
}
