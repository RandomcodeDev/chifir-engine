/// \file DirectX 12 device implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "rhi/irhidevice.h"
#include "rhi/rhi.h"

#include "dx12.h"

struct Dx12DeviceInfo_t
{
	IDXGIAdapter1* adapter;
	DXGI_ADAPTER_DESC1 desc;
};

class CDx12RhiInstance;

class CDx12RhiDevice: public IRhiDevice
{
  public:
	virtual void Destroy();

	virtual IRhiSwapChain* CreateSwapChain(u32 bufferCount);

	virtual IRhiCommandList* CreateCommandList(RhiCommandListFlags flags, ssize bufferSize)
	{
		UNUSED(flags);
		return nullptr;
	}
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
		u32 width, u32 height, u32 depth, RhiMemoryLocation location, RhiImageType type, RhiImageFormat format,
		RhiImageUsage usage)
	{
		UNUSED(width);
		UNUSED(height);
		UNUSED(depth);
		UNUSED(location);
		UNUSED(type);
		UNUSED(format);
		return nullptr;
	}
	virtual IRhiImageView* CreateImageView(IRhiImage* image)
	{
		UNUSED(image);
		return nullptr;
	}
	virtual IRhiRenderTarget* CreateRenderTarget(IRhiImageView* imageView)
	{
		UNUSED(imageView);
		return nullptr;
	}
	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc)
	{
		UNUSED(desc);
		return nullptr;
	}

	virtual void ExecuteCommandList(IRhiCommandList* cmdList)
	{
		ExecuteCommandLists(&cmdList, 1);
	}

	virtual void ExecuteCommandLists(IRhiCommandList** cmdLists, ssize count)
	{
		UNUSED(cmdLists);
		UNUSED(count);
	}

  private:
	friend class CDx12RhiInstance;
	friend class CDx12RhiSwapChain;

	CDx12RhiInstance* m_instance;
	Dx12DeviceInfo_t m_info;
	ID3D12Device4* m_handle;
	ID3D12CommandQueue* m_queue;
	ID3D12CommandQueue* m_copyQueue;

	CDx12RhiDevice(CDx12RhiInstance* instance, const Dx12DeviceInfo_t& info);
	bool Initialize();

	/// Get information about a device. Returns false if the device isn't usable.
	static bool GetDeviceInfo(RhiDeviceInfo_t& rhiInfo, Dx12DeviceInfo_t& info, IDXGIAdapter1* adapter, ssize i);

	CDx12RhiInstance* GetInstance()
	{
		return m_instance;
	}
};
