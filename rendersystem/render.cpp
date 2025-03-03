#include "rhi/irhidevice.h"
#include "rhi/irhiinstance.h"
#include "videosystem/ivideosystem.h"

#include "render.h"

bool CRenderSystem::Initialize(IVideoSystem* videoSystem)
{
	m_instance = Rhi_CreateInstance();
	if (!m_instance)
	{
		return false;
	}

	if (!m_instance->Initialize(videoSystem))
	{
		return false;
	}

	CVector<RhiDeviceInfo_t> deviceInfo;
	m_instance->GetDeviceInfo(deviceInfo);
	if (deviceInfo.Size() < 1)
	{
		return false;
	}

	m_device = m_instance->CreateDevice(deviceInfo[0]);
	if (!m_device)
	{
		return false;
	}

	return true;
}

void CRenderSystem::Shutdown()
{
	if (m_device)
	{
		m_device->Destroy();
		delete m_device;
		m_device = nullptr;
	}

	if (m_instance)
	{
		m_instance->Destroy();
		delete m_instance;
		m_instance = nullptr;
	}
}

void CRenderSystem::BeginFrame()
{
}

void CRenderSystem::EndFrame()
{
}
