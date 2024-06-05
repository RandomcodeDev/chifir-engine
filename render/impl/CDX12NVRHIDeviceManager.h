/// @file CDX12NVRHIDeviceManager.h
/// @brief This file declares the DirectX 12 device manager implementation for NVRHI
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include "framework/Framework.h"
#include "framework/Log.h"
#include "framework/Util.h"

#include "INVRHIDeviceManager.h"

#include "nvrhi/d3d12.h"

class CDX12NVRHIDeviceManager : public INVRHIDeviceManager
{
  public:
	CDX12NVRHIDeviceManager();
	~CDX12NVRHIDeviceManager();

	nvrhi::DeviceHandle CreateDevice();

	bool SetGpu(size_t index = 0);

	const std::string& GetGpuName()
	{
		return m_adapterName;
	}

	size_t GetGpuIndex()
	{
		return m_adapterIndex;
	}

	nvrhi::GraphicsAPI GetApi()
	{
		return nvrhi::GraphicsAPI::D3D12;
	}

  private:
	ComPtr<IDXGIFactory7> m_factory;
	std::vector<ComPtr<IDXGIAdapter4>> m_adapters;
	size_t m_adapterIndex;
	ComPtr<IDXGIAdapter4> m_adapter;
	std::string m_adapterName;
	DXGI_ADAPTER_DESC3 m_adapterDesc;
	ComPtr<ID3D12Device7> m_device;
	ComPtr<ID3D12CommandQueue> m_graphicsCmdQueue;
	ComPtr<ID3D12CommandQueue> m_transferCmdQueue;

	bool CreateFactory();
	bool GetAdapters();
	bool CreateD3dDevice();
	bool CreateCommandQueues();
	void Cleanup();
};
