/// \file DirectX 9 swap chain implementation
/// \copyright 2025 Randomcode Developers

#include "base/log.h"

#include "dx9.h"
#include "image_dx9.h"
#include "videosystem/ivideosystem.h"

#include "device_dx9.h"
#include "instance_dx9.h"
#include "swapchain_dx9.h"

CDx9RhiSwapChain::CDx9RhiSwapChain(CDx9RhiDevice* device) : CDx9RhiBaseObject(device)
{
}

bool CDx9RhiSwapChain::Initialize(u32 bufferCount)
{
	m_bufferCount = bufferCount;
	Log_Debug("Getting IDirect3DSwapChain9 and setting buffer count to %u", m_bufferCount);

	ResizeBuffers();

	return true;
}

void CDx9RhiSwapChain::Destroy()
{
	if (m_handle)
	{
		Log_Debug("Releasing IDirect3DSwapChain9 0x%016X", m_handle);
		m_handle->Release();
	}
}

void CDx9RhiSwapChain::GetBuffers(CVector<IRhiRenderTarget*>& buffers)
{
	buffers = m_buffers;
}

void CDx9RhiSwapChain::ResizeBuffers()
{
	auto& presentParams = m_parent->m_presentParams;
	presentParams.BackBufferCount = m_bufferCount;

	Log_Debug(
		"Resizing swap chain to %u %ux%u buffer(s)", presentParams.BackBufferCount, presentParams.BackBufferWidth,
		presentParams.BackBufferHeight);

	m_parent->m_handle->Reset(&presentParams);
	m_parent->m_handle->GetSwapChain(0, &m_handle);

	m_bufferCount = presentParams.BackBufferCount;
	Log_Debug("Got %u buffer(s)", m_bufferCount);

	m_buffers.Resize(m_bufferCount);
	for (ssize i = 0; i < m_buffers.Size(); i++)
	{
		IDirect3DSurface9* surface = nullptr;
		m_handle->GetBackBuffer(i, D3DBACKBUFFER_TYPE_MONO, &surface);
		m_buffers[i] = new CDx9RhiRenderTarget(m_parent, surface);
	}
}

u32 CDx9RhiSwapChain::Present()
{
	m_handle->Present(nullptr, nullptr, nullptr, nullptr, 0);
	return GetFrameIndex();
}
