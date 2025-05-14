/// \file DirectX 9 instance implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/loader.h"
#include "base/vector.h"

#include "device_dx9.h"
#include "rhi/irhidevice.h"
#include "rhi/irhiinstance.h"

#include "dx9.h"

class IVideoSystem;

class CDx9RhiInstance: public IRhiInstance
{
  public:
	CDx9RhiInstance() : m_d3d9Dll(nullptr), m_d3d9(nullptr)
	{
	}
	~CDx9RhiInstance() = default;

	virtual bool Initialize(IVideoSystem* videoSystem);
	virtual void Destroy();

	virtual void GetDeviceInfo(CVector<RhiDeviceInfo_t>& info);

	virtual IRhiDevice* CreateDevice(const RhiDeviceInfo_t& info);

  private:
	friend class CDx9RhiDevice;
	friend class CDx9RhiSwapChain;
    friend class CDx9RhiImage;

	IVideoSystem* m_videoSystem;
	ILibrary* m_d3d9Dll;
    IDirect3D9* m_d3d9;
	CVector<Dx9DeviceInfo_t> m_devices;

	/// Loads d3d9.dll
	bool LoadDlls();
};