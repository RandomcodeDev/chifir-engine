/// \file DirectX 8 instance implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/loader.h"
#include "base/vector.h"

#include "device_dx8.h"
#include "rhi/irhidevice.h"
#include "rhi/irhiinstance.h"

#include "dx8.h"

class IVideoSystem;

class CDx8RhiInstance: public IRhiInstance
{
  public:
	CDx8RhiInstance() : m_d3d8Dll(nullptr), m_d3d8(nullptr)
	{
	}
	~CDx8RhiInstance() = default;

	virtual bool Initialize(IVideoSystem* videoSystem);
	virtual void Destroy();

	virtual void GetDeviceInfo(CVector<RhiDeviceInfo_t>& info);

	virtual IRhiDevice* CreateDevice(const RhiDeviceInfo_t& info);

  private:
	friend class CDx8RhiDevice;
	friend class CDx8RhiSwapChain;
    friend class CDx8RhiImage;

	IVideoSystem* m_videoSystem;
	ILibrary* m_d3d8Dll;
    IDirect3D8* m_d3d8;
	CVector<Dx8DeviceInfo_t> m_devices;

	/// Loads d3d8.dll
	bool LoadDlls();
};