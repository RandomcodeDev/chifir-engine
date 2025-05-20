/// \file DirectX 8 device implementation
/// \copyright 2025 Randomcode Developers

#include "base/basicstr.h"
#include "base/log.h"
#include "base/platform.h"

#include "rhi/irhicommandlist.h"
#include "rhi/irhidevice.h"

#include "videosystem/ivideosystem.h"

#include "commandlist_dx8.h"
#include "dx8.h"
#include "device_dx8.h"
#include "image_dx8.h"
#include "instance_dx8.h"
#include "swapchain_dx8.h"

CDx8RhiDevice::CDx8RhiDevice(CDx8RhiInstance* instance, const Dx8DeviceInfo_t& info) : CDx8RhiBaseObject(instance), m_info(info)
{
}

bool CDx8RhiDevice::Initialize()
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
    m_presentParams.hDeviceWindow = reinterpret_cast<HWND>(video->GetHandle());
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
		Log_Debug("Creating IDirect3DDevice8 with device type %u (attempt %u/%u)", deviceType, attempts + 1, MAX_CREATE_ATTEMPTS);
		HRESULT result = m_parent->m_d3d8->CreateDevice(
			m_info.adapter, deviceType, m_presentParams.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_presentParams,
			&m_handle);
		if (FAILED(result))
		{
			Log_Error("IDirect3D8::CreateDevice failed: HRESULT 0x%08X", result);
			SetLastError(result);
			Log_Debug("Retrying in case present parameters were fixed");
			continue;
		}

		break;
	}

	return m_handle != nullptr;
}

void CDx8RhiDevice::Destroy()
{
	if (m_handle)
	{
		Log_Debug("Releasing IDirect3DDevice8 0x%016zX", m_handle);
		m_handle->Release();
		m_handle = nullptr;
	}
}

IRhiSwapChain* CDx8RhiDevice::CreateSwapChain(u32 bufferCount)
{
	CDx8RhiSwapChain* swapChain = new CDx8RhiSwapChain(this);
	if (!swapChain->Initialize(bufferCount))
	{
		delete swapChain;
		return nullptr;
	}

	return swapChain;
}

IRhiCommandList* CDx8RhiDevice::CreateCommandList(RhiCommandListFlags flags, ssize bufferSize)
{
    return new CDx8RhiCommandList(this, bufferSize);
}

IRhiImage* CDx8RhiDevice::CreateImage2d(
	u32 width, u32 height, u32 mipLevels, RhiMemoryLocation location, RhiImageType type, RhiImageFormat format,
	RhiImageUsage usage)
{
	CDx8RhiImage* image = new CDx8RhiImage(this, width, height, mipLevels, location, format, usage);
	if (!image->Initialize())
	{
		delete image;
		return nullptr;
	}

	return image;
}

IRhiImageView* CDx8RhiDevice::CreateImageView(IRhiImage* image)
{
	return nullptr; //new CDx8RhiImageView(this, image);
}

IRhiRenderTarget* CDx8RhiDevice::CreateRenderTarget(IRhiImageView* imageView)
{
	return new CDx8RhiRenderTarget(this, reinterpret_cast<CDx8RhiImageView*>(imageView));
}

void CDx8RhiDevice::ExecuteCommandLists(IRhiCommandList** cmdLists, ssize count)
{
	m_handle->BeginScene();
	for (ssize i = 0; i < count; i++)
	{
		reinterpret_cast<CDx8RhiCommandList*>(cmdLists[i])->Execute();
	}
	m_handle->EndScene();
}

bool CDx8RhiDevice::GetDeviceInfo(IDirect3D8* d3d8, RhiDeviceInfo_t& rhiInfo, Dx8DeviceInfo_t& info, u32 adapter)
{
	Log_Debug("Getting information for adapter %u", adapter);

	info.adapter = adapter;
	HRESULT result = d3d8->GetAdapterIdentifier(info.adapter, 0, &info.identifier);
	if (FAILED(result))
	{
		Log_Error("Failed to get adapter identifier for adapter %u: HRESULT 0x%08X", info.adapter, result);
		return false;
	}

	d3d8->GetAdapterDisplayMode(info.adapter, &info.mode);
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
