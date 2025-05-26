/// \file DirectX 8 device implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "rhi/irhicommandlist.h"
#include "rhi/irhidevice.h"
#include "rhi/rhi.h"

#include "dx8.h"

struct Dx8DeviceInfo_t
{
	u32 adapter;
	D3DADAPTER_IDENTIFIER8 identifier;
	D3DDISPLAYMODE mode;
};

class CDx8RhiInstance;

class CDx8RhiDevice: public IRhiDevice, CDx8RhiBaseObject<IDirect3DDevice8, CDx8RhiInstance>
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
	virtual IRhiBuffer* CreateBuffer(ssize size, RhiMemoryLocation location, RhiBufferUsage usage)
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
	friend class CDx8RhiInstance;
	friend class CDx8RhiSwapChain;
	friend class CDx8RhiImage;
	friend class CDx8RhiCommandList;

	Dx8DeviceInfo_t m_info;
	D3DPRESENT_PARAMETERS m_presentParams;
	CVector<Dx8DeviceInfo_t> m_devices;

	static constexpr u32 MAX_CREATE_ATTEMPTS = 3;

	CDx8RhiDevice(CDx8RhiInstance* instance, const Dx8DeviceInfo_t& info);
	bool Initialize();

	/// Get information about a device. Returns false if the device isn't usable.
	static bool GetDeviceInfo(IDirect3D8* d3d8, RhiDeviceInfo_t& rhiInfo, Dx8DeviceInfo_t& info, u32 adapter);
};
