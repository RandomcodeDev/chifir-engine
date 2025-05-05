/// \file DirectX 9 swap chain implementation
/// \copyright 2025 Randomcode Developers

#include "base/log.h"

#include "videosystem/ivideosystem.h"

#include "device_dx9.h"
#include "instance_dx9.h"
#include "swapchain_dx9.h"

CDx9RhiSwapChain::CDx9RhiSwapChain(CDx9RhiDevice* device) : m_device(device)
{
}

bool CDx9RhiSwapChain::Initialize(u32 bufferCount)
{
    Log_Debug("Getting IDirect3D9SwapChain9 and setting buffer count to %u", bufferCount);

    m_bufferCount = bufferCount;

    auto& presentParams = m_device->m_presentParams;
    presentParams.BackBufferCount = bufferCount;

    m_device->m_handle->Reset(&presentParams);
    m_device->m_handle->GetSwapChain(0, &m_handle);

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
}

void CDx9RhiSwapChain::ResizeBuffers()
{

}

u32 CDx9RhiSwapChain::Present()
{
    m_handle->Present(nullptr, nullptr, nullptr, nullptr, 0);
    return GetFrameIndex();
}
