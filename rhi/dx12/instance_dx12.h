/// \file DirectX 12 instance implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/loader.h"
#include "base/vector.h"

#include "device_dx12.h"
#include "rhi/irhidevice.h"
#include "rhi/irhiinstance.h"

#include "dx12.h"

class IVideoSystem;

class CDx12RhiInstance: public IRhiInstance
{
  public:
	CDx12RhiInstance() : m_dxgi(nullptr), m_factory(nullptr), m_hwnd(nullptr)
	{
	}
	~CDx12RhiInstance() DEFAULT;

	virtual bool Initialize(IVideoSystem* videoSystem);
	virtual void Destroy();

	virtual void GetDeviceInfo(CVector<RhiDeviceInfo_t>& info);

	virtual IRhiDevice* CreateDevice(const RhiDeviceInfo_t& info);

	static constexpr cstr DXGI_DLL_NAME =
#ifdef CH_GDKX
		"dxgi_xs";
#else
		"dxgi";
#endif
	static constexpr cstr D3D12_DLL_NAME =
#ifdef CH_GDKX
		"d3d12_xs";
#else
		"d3d12";
#endif

  private:
	friend class CDx12RhiDevice;
	friend class CDx12RhiSwapChain;

	IVideoSystem* m_videoSystem;
	ILibrary* m_dxgi;
	ILibrary* m_d3d12;
	IDXGIFactory6* m_factory;
	HWND m_hwnd;
	CVector<Dx12DeviceInfo_t> m_devices;

	/// Loads dxgi.dll and d3d12.dll
	bool LoadDlls();

	/// Tries to enable the debug layer, logging if it goes wrong
	void EnableDebugLayer();
};
