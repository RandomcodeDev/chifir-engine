#include "CDX12NVRHIDeviceManager.h"

CDX12NVRHIDeviceManager::CDX12NVRHIDeviceManager() : m_adapterDesc{}, m_adapterIndex(0)
{

}

CDX12NVRHIDeviceManager::~CDX12NVRHIDeviceManager()
{

}

nvrhi::DeviceHandle CDX12NVRHIDeviceManager::CreateDevice()
{
	if (!CreateFactory() || !GetAdapters() || !CreateD3dDevice() || !CreateCommandQueues())
	{
		return nullptr;
	}

	nvrhi::d3d12::DeviceDesc desc = {};
	desc.pDevice = m_device.Get();
	desc.pGraphicsCommandQueue = m_graphicsCmdQueue.Get();

	return nvrhi::d3d12::createDevice(desc);
}

bool CDX12NVRHIDeviceManager::CreateFactory()
{
	KR_LOG_DEBUG("Creating DXGI Factory");

	HRESULT result = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create DXGI factory: {}", GetHresultString(result));
		return false;
	}

	return true;
}

bool CDX12NVRHIDeviceManager::GetAdapters()
{
	KR_LOG_DEBUG("Enumerating adapters");

	HRESULT result;
	uint32_t i = 0;
	ComPtr<IDXGIAdapter1> adapter;
	while ((result = m_factory->EnumAdapters1(i++, adapter.GetAddressOf())) != DXGI_ERROR_NOT_FOUND)
	{
		if (!SUCCEEDED(result))
		{
			KR_LOG_ERROR("Failed to get adapter {}: {}", i, GetHresultString(result));
			continue;
		}

		adapter.As(&m_adapter);
		m_adapter->GetDesc3(&m_adapterDesc);
		KR_LOG_INFO(L"Got adapter {}: {} [{:04x}:{:04x}]", i, std::wstring_view(m_adapterDesc.Description), m_adapterDesc.VendorId, m_adapterDesc.DeviceId);
		m_adapters.push_back(m_adapter);
		m_adapter.Reset();
	}

	if (m_adapters.empty())
	{
		KR_LOG_ERROR("Couldn't get any adapters!");
		return false;
	}

	SetGpu();

	return true;
}

bool CDX12NVRHIDeviceManager::SetGpu(size_t index)
{
	if (index >= m_adapters.size())
	{
		return false;
	}

	m_adapterIndex = index;
	m_adapter = m_adapters[m_adapterIndex];
	m_adapter->GetDesc3(&m_adapterDesc);
	char adapterName[128] = {0};
	std::wcstombs(adapterName, m_adapterDesc.Description, KR_ARRAY_SIZE(adapterName));
	m_adapterName = adapterName;

	KR_LOG_INFO("GPU set to {} (index {})", m_adapterName, m_adapterIndex);

	return true;
}

bool CDX12NVRHIDeviceManager::CreateD3dDevice()
{
	HRESULT result = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create DirectX 12 device: {}", GetHresultString(result));
		return false;
	}

	return true;
}

bool CDX12NVRHIDeviceManager::CreateCommandQueues()
{
	KR_LOG_DEBUG("Creating command queues");

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	HRESULT result = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_graphicsCmdQueue));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create graphics command queue: {}", GetHresultString(result));
		return false;
	}

	desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	result = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_graphicsCmdQueue));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create graphics command queue: {}", GetHresultString(result));
		return false;
	}

	return true;
}

void CDX12NVRHIDeviceManager::Cleanup()
{
}
