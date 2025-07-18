/// \file DirectX 9 swap chain implementation
/// \copyright 2025 Randomcode Developers

#include "base/log.h"

#include "videosystem/ivideosystem.h"

#include "device_dx9.h"
#include "dx9.h"
#include "image_dx9.h"
#include "instance_dx9.h"
#include "swapchain_dx9.h"


CDx9RhiSwapChain::CDx9RhiSwapChain(CDx9RhiDevice* device) : CDxRhiBaseObject(device)
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
    presentParams.BackBufferCount = (UINT)m_bufferCount;
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

    HRESULT result = m_parent->m_handle->Reset(&presentParams);
    if (!SUCCEEDED(result))
    {
        Base_Quit("Failed to reset device: HRESULT 0x%08X", result);
    }

    m_parent->m_handle->GetSwapChain(0, &m_handle);

    m_buffers.Resize(m_bufferCount);
    for (ssize i = 0; i < m_bufferCount; i++)
    {
        IDirect3DSurface9* surface = nullptr;
        result = m_handle->GetBackBuffer((UINT)i, D3DBACKBUFFER_TYPE_MONO, &surface);
        if (FAILED(result))
        {
            if (m_buffers.Size() < 1)
            {
                Base_Quit("Failed to get backbuffer %zd: HRESULT 0x%08X", i, result);
            }
            break;
        }
        m_buffers[i] = new CDx9RhiRenderTarget(m_parent, surface);
    }

    Log_Debug("Got %u buffer(s)", m_bufferCount);
}

u32 CDx9RhiSwapChain::Present()
{
    m_handle->Present(nullptr, nullptr, nullptr, nullptr, 0);
    return GetFrameIndex();
}
