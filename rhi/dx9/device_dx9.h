/// \file DirectX 9 device implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "rhi/irhidevice.h"
#include "rhi/rhi.h"

#include "dx9.h"

struct Dx9DeviceInfo_t
{
	u32 adapter;
    D3DADAPTER_IDENTIFIER9 identifier;
    D3DDISPLAYMODE mode;
};

class CDx9RhiInstance;

class CDx9RhiDevice: public IRhiDevice
{
  public:
	virtual void Destroy();

	virtual IRhiSwapChain* CreateSwapChain(u32 bufferCount);

	virtual IRhiCommandList* CreateCommandList(RhiCommandListFlags_t flags)
	{
		UNUSED(flags);
		return nullptr;
	}
	virtual IRhiFence* CreateFence(u64 initialValue = 0)
	{
		UNUSED(initialValue);
		return nullptr;
	}
	virtual IRhiBuffer* CreateBuffer(ssize size, RhiMemoryLocation_t location, RhiMemoryUsage_t usage)
	{
		UNUSED(size);
		UNUSED(location);
		UNUSED(usage);
		return nullptr;
	}
	virtual IRhiImage* CreateImage(
		u32 width, u32 height, u32 depth, RhiMemoryLocation_t location, RhiImageType type, RhiImageFormat format)
	{
		UNUSED(width);
		UNUSED(height);
		UNUSED(depth);
		UNUSED(location);
		UNUSED(type);
		UNUSED(format);
		return nullptr;
	}
	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc)
	{
		UNUSED(desc);
		return nullptr;
	}

  private:
	friend class CDx9RhiInstance;
	friend class CDx9RhiSwapChain;

	CDx9RhiInstance* m_instance;
	Dx9DeviceInfo_t m_info;
	IDirect3DDevice9* m_handle;
	D3DPRESENT_PARAMETERS m_presentParams;
	CVector<Dx9DeviceInfo_t> m_devices;

    static constexpr u32 MAX_CREATE_ATTEMPTS = 3;

	CDx9RhiDevice(CDx9RhiInstance* instance, const Dx9DeviceInfo_t& info);
	bool Initialize();

	/// Get information about a device. Returns false if the device isn't usable.
	static bool GetDeviceInfo(IDirect3D9* d3d9, RhiDeviceInfo_t& rhiInfo, Dx9DeviceInfo_t& info, u32 adapter);

	CDx9RhiInstance* GetInstance()
	{
		return m_instance;
	}
};
