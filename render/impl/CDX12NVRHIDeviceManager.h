/// @file CDX12NVRHIDeviceManager.h
/// @brief This file declares the DirectX 12 device manager implementation for NVRHI
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include "INVRHIDeviceManager.h"
#include "framework/Framework.h"
#include "framework/Log.h"
#include "framework/Util.h"

#include "nvrhi/d3d12.h"

class CDX12NVRHIDeviceManager : public INVRHIDeviceManager
{
  public:
	nvrhi::DeviceHandle CreateDevice();
	bool CreateSwapChain(IVideoSystem* videoSystem);
	void DestroySwapChain();

	void BeginFrame(IVideoSystem* videoSystem);
	void Present();

	bool SetGpu(size_t index = 0);

	const std::string& GetGpuName()
	{
		return m_adapterName;
	}

	size_t GetGpuIndex()
	{
		return m_adapterIndex;
	}

  private:
	nvrhi::d3d12::DeviceHandle m_rhiDevice;

	ComPtr<IDXGIFactory7> m_factory;
	std::vector<ComPtr<IDXGIAdapter4>> m_adapters;
	size_t m_adapterIndex;
	ComPtr<IDXGIAdapter4> m_adapter;
	std::string m_adapterName;
	DXGI_ADAPTER_DESC3 m_adapterDesc;
	ComPtr<ID3D12Device7> m_device;
	ComPtr<ID3D12CommandQueue> m_graphicsCmdQueue;
	ComPtr<ID3D12CommandQueue> m_copyCmdQueue;
	ComPtr<IDXGISwapChain4> m_swapChain;
	std::vector<ComPtr<ID3D12Resource>> m_rawSwapChainBuffers;
	std::vector<nvrhi::TextureHandle> m_swapChainBuffers;
	DXGI_SWAP_CHAIN_DESC1 m_swapChainDesc;
	ComPtr<ID3D12Fence> m_fence;
	std::vector<HANDLE> m_fenceEvents;

	bool CreateFactory();
	bool GetAdapters();
	bool CreateD3dDevice();
	bool CreateCommandQueues();
	bool CreateRenderTargets();
	void ReleaseRenderTargets();
	void ResizeSwapChain(u32 width, u32 height);
};
