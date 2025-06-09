/// \file DirectX 9 device implementation
/// \copyright 2025 Randomcode Developers

#include "base/basicstr.h"
#include "base/log.h"
#include "base/platform.h"

#include "rhi/irhicommandlist.h"
#include "rhi/irhidevice.h"

#include "videosystem/ivideosystem.h"

#include "commandlist_dx.h"
#include "dx9.h"
#include "device_dx9.h"
#include "image_dx9.h"
#include "instance_dx9.h"
#include "swapchain_dx9.h"

CDx9RhiDevice::CDx9RhiDevice(CDx9RhiInstance* instance, const Dx9DeviceInfo_t& info) : CDxRhiBaseObject(instance), m_info(info)
{
}

bool CDx9RhiDevice::Initialize()
{
	IVideoSystem* video = m_parent->m_videoSystem;

	Base_MemSet(&m_presentParams, 0, sizeof(D3DPRESENT_PARAMETERS));
	m_presentParams.BackBufferWidth = video->GetWidth();
	m_presentParams.BackBufferHeight = video->GetHeight();
	m_presentParams.BackBufferFormat = m_info.mode.Format;
	m_presentParams.BackBufferCount = 1;
	m_presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
#ifdef CH_XBOX
    m_presentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_UNLIMITED;
    m_presentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;
#else
    m_presentParams.Windowed = true;
    HWND window = nullptr;
    if (Plat_IsUwpApp())
    {
        auto coreWindow = reinterpret_cast<winrt_min::ICoreWindow*>(video->GetHandle());
        winrt_min::ICoreWindowInterop* interop;
        reinterpret_cast<IUnknown*>(coreWindow)->QueryInterface(&interop);
        interop->WindowHandle(&window);
    }
    else
    {
        window = reinterpret_cast<HWND>(video->GetHandle());
    }
    m_presentParams.hDeviceWindow = window;
    m_presentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#endif

	// try multiple times, it's possible that it will fix things
	for (u32 attempts = 0; attempts < MAX_CREATE_ATTEMPTS; attempts++)
	{
		D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
		if (attempts >= 2)
		{
			deviceType = D3DDEVTYPE_REF;
		}
		Log_Debug("Creating IDirect3DDevice9 with device type %u (attempt %u/%u)", deviceType, attempts + 1, MAX_CREATE_ATTEMPTS);
		HRESULT result = m_parent->m_d3d9->CreateDevice(
			m_info.adapter, deviceType, m_presentParams.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_presentParams,
			&m_handle);
		if (FAILED(result))
		{
			Log_Error("IDirect3D9::CreateDevice failed: HRESULT 0x%08X", result);
			LastNtError() = result;
			LastNtStatus() = 0;
			Log_Debug("Retrying in case present parameters were fixed");
			continue;
		}

		break;
	}

	return m_handle != nullptr;
}

void CDx9RhiDevice::Destroy()
{
	if (m_handle)
	{
		Log_Debug("Releasing IDirect3DDevice9 0x%016zX", m_handle);
		m_handle->Release();
		m_handle = nullptr;
	}
}

IRhiSwapChain* CDx9RhiDevice::CreateSwapChain(u32 bufferCount)
{
	CDx9RhiSwapChain* swapChain = new CDx9RhiSwapChain(this);
	if (!swapChain->Initialize(bufferCount))
	{
		delete swapChain;
		return nullptr;
	}

	return swapChain;
}

IRhiCommandList* CDx9RhiDevice::CreateCommandList(RhiCommandListFlags flags, ssize bufferSize)
{
    return new CDxRhiCommandList(this, bufferSize);
}

IRhiImage* CDx9RhiDevice::CreateImage2d(
	u32 width, u32 height, u32 mipLevels, RhiMemoryLocation location, RhiImageType type, RhiImageFormat format,
	RhiImageUsage usage)
{
	CDx9RhiImage* image = new CDx9RhiImage(this, width, height, mipLevels, location, format, usage);
	if (!image->Initialize())
	{
		delete image;
		return nullptr;
	}

	return image;
}

IRhiImageView* CDx9RhiDevice::CreateImageView(IRhiImage* image)
{
	return nullptr; //new CDx9RhiImageView(this, image);
}

IRhiRenderTarget* CDx9RhiDevice::CreateRenderTarget(IRhiImageView* imageView)
{
	return new CDx9RhiRenderTarget(this, reinterpret_cast<CDx9RhiImageView*>(imageView));
}

void CDx9RhiDevice::ExecuteCommandLists(IRhiCommandList** cmdLists, ssize count)
{
	m_handle->BeginScene();
	for (ssize i = 0; i < count; i++)
	{
		reinterpret_cast<CDxRhiCommandList*>(cmdLists[i])->Execute();
	}
	m_handle->EndScene();
}

bool CDx9RhiDevice::GetDeviceInfo(IDirect3D9* d3d9, RhiDeviceInfo_t& rhiInfo, Dx9DeviceInfo_t& info, u32 adapter)
{
	Log_Debug("Getting information for adapter %u", adapter);

	info.adapter = adapter;
	HRESULT result = d3d9->GetAdapterIdentifier(info.adapter, 0, &info.identifier);
	if (FAILED(result))
	{
		Log_Error("Failed to get adapter identifier for adapter %u: HRESULT 0x%08X", info.adapter, result);
		return false;
	}

	d3d9->GetAdapterDisplayMode(info.adapter, &info.mode);
	Log_Debug("Adapter %u's format is %u", info.adapter, info.mode.Format);

	rhiInfo.handle = adapter;

	rhiInfo.name = info.identifier.Description;

	// this basically only matters in vulkan, but still fill it out
	rhiInfo.deviceType = RhiDeviceType::Discrete;

	rhiInfo.vendorId = info.identifier.VendorId;
	rhiInfo.deviceId = info.identifier.DeviceId;

	rhiInfo.totalMemory = 0;

	return true;
}
