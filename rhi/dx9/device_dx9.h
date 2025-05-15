/// \file DirectX 9 device implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "rhi/irhicommandlist.h"
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

class CDx9RhiDevice: public IRhiDevice, CDx9RhiBaseObject<IDirect3DDevice9, CDx9RhiInstance>
{
  public:
	virtual void Destroy();

	virtual IRhiSwapChain* CreateSwapChain(u32 bufferCount);

	virtual IRhiCommandList* CreateCommandList(RhiCommandListFlags flags, ssize bufferSize);

	virtual IRhiFence* CreateFence(u64 initialValue = 0)
	{
		UNUSED(initialValue);
		return nullptr;
	}
	virtual IRhiBuffer* CreateBuffer(ssize size, RhiMemoryLocation location, RhiMemoryUsage usage)
	{
		UNUSED(size);
		UNUSED(location);
		UNUSED(usage);
		return nullptr;
	}

	virtual IRhiImage* CreateImage2d(
		u32 width, u32 height, u32 mipLevels, RhiMemoryLocation location, RhiImageType type, RhiImageFormat format,
		RhiImageUsage usage);
	virtual IRhiImageView* CreateImageView(IRhiImage* image);
	virtual IRhiRenderTarget* CreateRenderTarget(IRhiImageView* imageView);

	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc)
	{
		UNUSED(desc);
		return nullptr;
	}

	virtual void ExecuteCommandList(IRhiCommandList* cmdList)
	{
		ExecuteCommandLists(&cmdList, 1);
	}

	virtual void ExecuteCommandLists(IRhiCommandList** cmdLists, ssize count);

  private:
	// TODO: proper getters, this is ugly
	friend class CDx9RhiInstance;
	friend class CDx9RhiSwapChain;
	friend class CDx9RhiImage;
	friend class CDx9RhiCommandList;

	Dx9DeviceInfo_t m_info;
	D3DPRESENT_PARAMETERS m_presentParams;
	CVector<Dx9DeviceInfo_t> m_devices;

	static constexpr u32 MAX_CREATE_ATTEMPTS = 3;

	CDx9RhiDevice(CDx9RhiInstance* instance, const Dx9DeviceInfo_t& info);
	bool Initialize();

	/// Get information about a device. Returns false if the device isn't usable.
	static bool GetDeviceInfo(IDirect3D9* d3d9, RhiDeviceInfo_t& rhiInfo, Dx9DeviceInfo_t& info, u32 adapter);
};
