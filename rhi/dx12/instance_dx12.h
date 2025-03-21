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

	template<typename T>
	T GetD3d12Symbol(cstr name)
	{
		return m_d3d12->GetSymbol<T>(name);
	}

	static constexpr cstr DXGI_DLL_NAME =
#ifdef CH_GDKX
		"d3d12_xs";
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
	ILibrary* m_dxgi;
	ILibrary* m_d3d12;
	IDXGIFactory6* m_factory;
	HWND m_hwnd;
	CVector<Dx12DeviceInfo_t> m_devices;
};
