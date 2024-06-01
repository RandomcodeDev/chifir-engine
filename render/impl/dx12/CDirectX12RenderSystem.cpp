#include "framework/Log.h"
#include "framework/Util.h"

#include "CDirectX12RenderSystem.h"

bool CDirectX12RenderSystem::Initialize()
{
	KR_LOG_INFO("Initializing DirectX 12 render system");

	if (!CreateFactory() || !FindAdapter() || !CreateDevice())
	{
		return false;
	}

	KR_LOG_INFO("DirectX 12 render system initialized");
	return true;
}

void CDirectX12RenderSystem::WaitForGpu()
{
}

void CDirectX12RenderSystem::Shutdown()
{
	KR_LOG_INFO("Shutting down DirectX 12 render system");



	KR_LOG_INFO("DirectX 12 render system shut down");
}

void CDirectX12RenderSystem::BeginFrame()
{
}

void CDirectX12RenderSystem::EndFrame()
{
}

void CDirectX12RenderSystem::ClearColour(rtm::vector4d colour)
{
}

void CDirectX12RenderSystem::EnableDebug()
{
#ifdef KR_DEBUG
#endif
}

bool CDirectX12RenderSystem::CreateFactory()
{
	HRESULT result = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create DXGI factory: HRESULT 0x{:08x}!", result);
		return false;
	}

	return true;
}

bool CDirectX12RenderSystem::FindAdapter()
{
	HRESULT result = m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to get high performance adapter: HRESULT 0x{:08x}!", result);
		return false;
	}

	m_adapter->GetDesc1(&m_adapterDesc);
	char adapterName[128] = {};
	wcstombs(adapterName, m_adapterDesc.Description, KR_ARRAYSIZE(adapterName));
	m_adapterName = adapterName;

	return true;
}

bool CDirectX12RenderSystem::CreateDevice()
{
	HRESULT result = D3D12CreateDevice(m_adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create device: HRESULT 0x{:08x}!", result);
		return false;
	}

	return true;
}
