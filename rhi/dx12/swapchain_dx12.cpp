/// \file DirectX 12 swap chain implementation
/// \copyright 2025 Randomcode Developers

#include "base/log.h"

#include "videosystem/ivideosystem.h"

#include "device_dx12.h"
#include "instance_dx12.h"
#include "swapchain_dx12.h"

CDx12RhiSwapChain::CDx12RhiSwapChain(CDx12RhiDevice* device) : m_device(device)
{
}

bool CDx12RhiSwapChain::Initialize(u32 bufferCount)
{
	m_bufferCount = bufferCount;

	IVideoSystem* video = m_device->GetInstance()->m_videoSystem;
	Log_Debug("Creating IDXGISwapChain4 with %u %ux%u buffers", bufferCount, video->GetWidth(), video->GetHeight());

	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = video->GetWidth();
	desc.Height = video->GetHeight();
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = FALSE;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = bufferCount;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	desc.SampleDesc.Count = 1;

	IDXGISwapChain1* swapChain;
	HRESULT result = S_OK;
	cstr function = nullptr;
#ifdef CH_UWP
	if (Plat_IsUwpApp())
	{
		function = "CreateSwapChainForCoreWindow";
		result = m_device->GetInstance()->m_factory->CreateSwapChainForCoreWindow(
			m_device->m_queue, reinterpret_cast<IUnknown*>(video->GetHandle()), &desc, nullptr, &swapChain);
	}
	else
#endif
	{
		function = "CreateSwapChainForHwnd";
		result = m_device->GetInstance()->m_factory->CreateSwapChainForHwnd(
			m_device->m_queue, reinterpret_cast<HWND>(video->GetHandle()), &desc, nullptr, nullptr, &swapChain);
	}

	if (FAILED(result))
	{
		Log_Error("%s failed: HRESULT 0x%08X", function, result);
		return false;
	}

	swapChain->QueryInterface(&m_handle);
	swapChain->Release();

	return true;
}

void CDx12RhiSwapChain::Destroy()
{
	if (m_handle)
	{
		Log_Debug("Releasing IDXGISwapChain4 0x%016X", m_handle);
		m_handle->Release();
	}
}

void CDx12RhiSwapChain::GetBuffers(CVector<IRhiRenderTarget*>& buffers)
{
}

void CDx12RhiSwapChain::ResizeBuffers()
{
	IVideoSystem* video = m_device->GetInstance()->m_videoSystem;
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	m_handle->GetDesc1(&desc);
	m_handle->ResizeBuffers(m_bufferCount, video->GetWidth(), video->GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, desc.Flags);
	Log_Debug("Resized %u buffers to %ux%u", m_bufferCount, video->GetWidth(), video->GetHeight());
}

u32 CDx12RhiSwapChain::Present()
{
	m_handle->Present(0, 0);
	return GetFrameIndex();
}
