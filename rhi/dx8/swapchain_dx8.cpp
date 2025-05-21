/// \file DirectX 8 swap chain implementation
/// \copyright 2025 Randomcode Developers

#include "base/log.h"

#include "dx8.h"
#include "image_dx8.h"
#include "videosystem/ivideosystem.h"

#include "device_dx8.h"
#include "instance_dx8.h"
#include "swapchain_dx8.h"

CDx8RhiSwapChain::CDx8RhiSwapChain(CDx8RhiDevice* device) : CDx8RhiBaseObject(device)
{
}

bool CDx8RhiSwapChain::Initialize(u32 bufferCount)
{
	m_bufferCount = bufferCount;
	Log_Debug("Getting IDirect3DSwapChain8 and setting buffer count to %u", m_bufferCount);

	ResizeBuffers();

	return true;
}

void CDx8RhiSwapChain::Destroy()
{
}

void CDx8RhiSwapChain::GetBuffers(CVector<IRhiRenderTarget*>& buffers)
{
	buffers = m_buffers;
}

void CDx8RhiSwapChain::ResizeBuffers()
{
	auto& presentParams = m_parent->m_presentParams;
	presentParams.BackBufferCount = m_bufferCount;
	auto videoSystem = m_parent->m_parent->m_videoSystem;
	presentParams.BackBufferWidth = videoSystem->GetWidth();
	presentParams.BackBufferHeight = videoSystem->GetHeight();

	Log_Debug(
		"Resizing swap chain to %u %ux%u buffer(s)", presentParams.BackBufferCount, presentParams.BackBufferWidth,
		presentParams.BackBufferHeight);

	for (ssize i = 0; i < m_buffers.Size(); i++)
	{
		if (m_buffers[i])
		{
			m_buffers[i]->Destroy();
			delete m_buffers[i];
			m_buffers[i] = nullptr;
		}
	}

    auto device = m_parent->m_handle;
	device->Reset(&presentParams);

	m_buffers.Empty();
	HRESULT result;
	do
	{
		IDirect3DSurface8* surface = nullptr;
		result = device->GetBackBuffer(m_buffers.Size(), D3DBACKBUFFER_TYPE_MONO, &surface);
		if (FAILED(result))
		{
			if (m_buffers.Size() < 1)
			{
				Base_Quit("Failed to get backbuffer %zd: HRESULT 0x%08X", m_buffers.Size(), result);
			}
			break;
		}
		m_buffers.Add(new CDx8RhiRenderTarget(m_parent, surface));
	} while (SUCCEEDED(result));

	m_bufferCount = m_buffers.Size();
	Log_Debug("Got %u buffer(s)", m_bufferCount);
}

u32 CDx8RhiSwapChain::Present()
{
    auto device = m_parent->m_handle;
	device->Present(nullptr, nullptr, nullptr, nullptr);
	return GetFrameIndex();
}
