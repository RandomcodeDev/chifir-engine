/// @file CDirectX12RenderSystem.h
/// @brief This file defines the DirectX 12 implementation of the render system
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include <dxgi1_6.h>
#ifdef KR_DEBUG
#include <dxgidebug.h>
#endif

#include "framework/Framework.h"
#include "render/IRenderSystem.h"

class CDirectX12RenderSystem : public IRenderSystem
{
  public:
	CDirectX12RenderSystem() = default;
	~CDirectX12RenderSystem() = default;
	bool Initialize();
	void WaitForGpu();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	void ClearColour(rtm::vector4d colour);

	const std::string& GetName() const
	{
		return NAME;
	}

	const std::string& GetGpuName() const
	{
		return m_adapterName;
	}

  private:
	static constexpr const char* NAME = "DirectX 12";

#ifdef KR_DEBUG
	IDXGIDebug* m_debug;
#endif
	IDXGIFactory6* m_factory;
	IDXGIAdapter1* m_adapter;
	std::string m_adapterName;
	ID3D12Device4* m_device;

	void EnableDebug();
	bool CreateFactory();
	bool FindAdapter();
	bool CreateDevice();
};
