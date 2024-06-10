#include "CDX12NVRHIDeviceManager.h"
#include "framework/IPlatform.h"
#include "render/IRenderSystem.h"
#include "video/IVideoSystem.h"

nvrhi::DeviceHandle CDX12NVRHIDeviceManager::CreateDevice()
{
	KR_LOG_INFO("Creating Direct3D 12 NVRHI device");

	if (!CreateFactory() || !GetAdapters() || !CreateD3dDevice() || !CreateCommandQueues())
	{
		return nullptr;
	}

	nvrhi::d3d12::DeviceDesc desc = {};
	desc.pDevice = m_device.Get();
	desc.pGraphicsCommandQueue = m_graphicsCmdQueue.Get();

	m_rhiDevice = nvrhi::d3d12::createDevice(desc);
	return m_rhiDevice;
}

bool CDX12NVRHIDeviceManager::CreateSwapChain(IVideoSystem* videoSystem)
{
	KR_LOG_DEBUG("Creating swap chain");

	std::memset(&m_swapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));
	m_swapChainDesc.Width = videoSystem->GetWidth();
	m_swapChainDesc.Height = videoSystem->GetHeight();
	m_swapChainDesc.SampleDesc.Count = 1;
	m_swapChainDesc.SampleDesc.Quality = 0;
	m_swapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_swapChainDesc.BufferCount = IRenderSystem::FRAMES_IN_FLIGHT;
	m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// m_swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	m_swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	m_swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

	ComPtr<IDXGISwapChain1> swapChain;
	HRESULT result = m_factory->CreateSwapChainForHwnd(
		m_graphicsCmdQueue.Get(), (HWND)videoSystem->GetHandle(), &m_swapChainDesc, nullptr, nullptr, swapChain.GetAddressOf());
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create swap chain: {}", GetHresultString(result));
		return false;
	}

	result = swapChain.As(&m_swapChain);
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to get swap chain as IDXGISwapChain4: {}", GetHresultString(result));
		return false;
	}

	KR_LOG_DEBUG("Creating fence and events");
	result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf()));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Faield to create fence: {}", GetHresultString(result));
		return false;
	}

	for (u32 i = 0; i < m_swapChainDesc.BufferCount; i++)
	{
		HANDLE event = CreateEventA(nullptr, false, true, nullptr);
		if (!event)
		{
			KR_LOG_ERROR("Failed to create fence {}: {}", i, GetHresultString(HRESULT_FROM_WIN32(GetLastError())));
			for (HANDLE e : m_fenceEvents)
			{
				CloseHandle(e);
			}
			return false;
		}
		m_fenceEvents.push_back(event);
	}

	if (!CreateRenderTargets())
	{
		return false;
	}

	return true;
}

void CDX12NVRHIDeviceManager::ResizeSwapChain(u32 width, u32 height)
{
	KR_LOG_DEBUG("Resizing swap chain");

	ReleaseRenderTargets();

	m_swapChainDesc.Width = width;
	m_swapChainDesc.Height = height;
	HRESULT result = m_swapChain->ResizeBuffers(
		m_swapChainDesc.BufferCount, m_swapChainDesc.Width, m_swapChainDesc.Height, m_swapChainDesc.Format, m_swapChainDesc.Flags);
	if (!SUCCEEDED(result))
	{
		KR_QUIT("Failed to resize swap chain");
	}

	if (!CreateRenderTargets())
	{
		KR_QUIT("Failed to re-create render targets");
	}
}

void CDX12NVRHIDeviceManager::DestroySwapChain()
{
	ReleaseRenderTargets();
	m_swapChain->Release();
}

void CDX12NVRHIDeviceManager::BeginFrame(IVideoSystem* videoSystem)
{
	if (videoSystem->Resized())
	{
		ResizeSwapChain(videoSystem->GetWidth(), videoSystem->GetHeight());
	}

	u32 bufferIdx = m_swapChain->GetCurrentBackBufferIndex();
	WaitForSingleObject(m_fenceEvents[bufferIdx], INFINITE);
}

void CDX12NVRHIDeviceManager::Present()
{
	u32 bufferIdx = m_swapChain->GetCurrentBackBufferIndex();

	m_swapChain->Present(0, 0);

	m_fence->SetEventOnCompletion(IRenderSystem::FRAMES_IN_FLIGHT, m_fenceEvents[bufferIdx]);
	m_graphicsCmdQueue->Signal(m_fence.Get(), IRenderSystem::FRAMES_IN_FLIGHT);
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
		KR_LOG_INFO(
			L"Got adapter {}: {} [{:04x}:{:04x}]", i, std::wstring_view(m_adapterDesc.Description), m_adapterDesc.VendorId, m_adapterDesc.DeviceId);
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
	HRESULT result = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_device.GetAddressOf()));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create Direct3D 12 device: {}", GetHresultString(result));
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
	result = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_copyCmdQueue));
	if (!SUCCEEDED(result))
	{
		KR_LOG_ERROR("Failed to create graphics command queue: {}", GetHresultString(result));
		return false;
	}

	return true;
}

bool CDX12NVRHIDeviceManager::CreateRenderTargets()
{
	KR_LOG_DEBUG("Creating render targets");

	m_rawSwapChainBuffers.resize(m_swapChainDesc.BufferCount);
	m_swapChainBuffers.resize(m_swapChainDesc.BufferCount);

	for (u32 i = 0; i < m_swapChainDesc.BufferCount; i++)
	{
		HRESULT result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_rawSwapChainBuffers[i].GetAddressOf()));
		if (!SUCCEEDED(result))
		{
			KR_LOG_ERROR("Failed to get buffer {}: {}", i, GetHresultString(result));
			return false;
		}

		nvrhi::TextureDesc textureDesc = {};
		textureDesc.width = m_swapChainDesc.Width;
		textureDesc.height = m_swapChainDesc.Height;
		textureDesc.sampleCount = m_swapChainDesc.SampleDesc.Count;
		textureDesc.sampleQuality = m_swapChainDesc.SampleDesc.Quality;
		textureDesc.format = nvrhi::Format::RGBA8_UNORM;
		textureDesc.debugName = fmt::format("Swap chain buffer {}", i);
		textureDesc.isRenderTarget = true;
		textureDesc.isUAV = false;
		textureDesc.initialState = nvrhi::ResourceStates::Present;
		textureDesc.keepInitialState = true;

		m_swapChainBuffers[i] =
			m_rhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(m_rawSwapChainBuffers[i].Get()), textureDesc);
	}

	return true;
}

void CDX12NVRHIDeviceManager::ReleaseRenderTargets()
{
	m_rhiDevice->waitForIdle();
	m_rhiDevice->runGarbageCollection();

	for (HANDLE event : m_fenceEvents)
	{
		SetEvent(event);
	}

	m_swapChainBuffers.clear();
	m_rawSwapChainBuffers.clear();
}
