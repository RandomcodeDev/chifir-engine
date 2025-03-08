#include "base/log.h"

#include "rhi/irhidevice.h"
#include "rhi/irhiinstance.h"

#include "videosystem/ivideosystem.h"

#include "render.h"

bool CRenderSystem::Initialize(IVideoSystem* videoSystem)
{
	m_instance = Rhi_CreateInstance();
	if (!m_instance)
	{
		Shutdown();
		return false;
	}

	if (!m_instance->Initialize(videoSystem))
	{
		Shutdown();
		return false;
	}

	if (!ChangeDevice(0))
	{
		return false;
	}

	return true;
}

void CRenderSystem::Shutdown()
{
	if (m_swapChain)
	{
		m_swapChain->Destroy();
		delete m_swapChain;
		m_swapChain = nullptr;
	}

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

bool CRenderSystem::ChangeDevice(u32 index)
{
	Log_Info("Render system switching to device %u", index);

	CVector<RhiDeviceInfo_t> deviceInfo;
	m_instance->GetDeviceInfo(deviceInfo);
	if (deviceInfo.Size() < 1)
	{
		Log_Error("No usable devices available!");
		Shutdown();
		return false;
	}

	if (index >= deviceInfo.Size())
	{
		Log_Warning("Device %u doesn't exist, using device 0 instead", index);
		index = 0;
	}

	m_device = m_instance->CreateDevice(deviceInfo[index]);
	if (!m_device)
	{
		Shutdown();
		return false;
	}

	m_swapChain = m_device->CreateSwapChain(3, m_swapChainBuffers);
	if (!m_swapChain)
	{
		Shutdown();
		return false;
	}

	return true;
}

void CRenderSystem::BeginFrame()
{
}

void CRenderSystem::EndFrame()
{
}
