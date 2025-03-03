#pragma once

#include "base/vector.h"

#include "rhi/irhidevice.h"
#include "rhi/irhiinstance.h"

#include "dx12.h"

class ILibrary;
class IVideoSystem;

class CDx12RhiInstance: public IRhiInstance
{
  public:
	CDx12RhiInstance() : m_factory(nullptr), m_hwnd(nullptr)
	{
	}
	~CDx12RhiInstance() DEFAULT;

	virtual bool Initialize(IVideoSystem* videoSystem);
	virtual void Destroy();

	virtual void GetDeviceInfo(CVector<RhiDeviceInfo_t>& info);

	virtual IRhiDevice* CreateDevice(const RhiDeviceInfo_t& info);

  private:
	IDXGIFactory6* m_factory;
	HWND m_hwnd;
};
