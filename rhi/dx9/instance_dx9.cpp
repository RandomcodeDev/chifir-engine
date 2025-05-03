/// \file DirectX 9 instance implementation
/// \copyright 2025 Randomcode Developers

#include "base/log.h"

#include "videosystem/ivideosystem.h"

#include "instance_dx9.h"

bool CDx9RhiInstance::LoadDlls()
{
    Log_Debug("Loading DirectX 9 runtime d3d9.dll");
    m_d3d9Dll = Base_LoadLibrary("d3d9");
    if (!m_d3d9Dll)
    {
        Log_Error("Failed to load D3D9 runtime");
        Destroy();
        return false;
    }

    return true;
}

bool CDx9RhiInstance::Initialize(IVideoSystem* videoSystem)
{
    m_videoSystem = videoSystem;

    if (!LoadDlls())
    {
        return false;
    }

    Log_Debug("Getting address of Direct3DCreate9");
    auto f_Direct3DCreate9 = GET_SYMBOL(m_d3d9Dll, Direct3DCreate9);
    if (!f_Direct3DCreate9)
    {
        Log_Error("Failed to get Direct3DCreate9");
        Destroy();
        return false;
    }

    Log_Debug("Creating IDirect3D9 with SDK version %u", D3D_SDK_VERSION);
    m_d3d9 = f_Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_d3d9)
    {
        Log_Error("Direct3DCreate9 failed");
        Destroy();
        return false;
    }

    return true;
}

void CDx9RhiInstance::Destroy()
{
    if (m_d3d9)
    {
        Log_Debug("Releasing IDirect3D9 0x%016zX", reinterpret_cast<uptr>(m_d3d9));
        m_d3d9->Release();
        m_d3d9 = nullptr;
    }

    if (m_d3d9Dll)
    {
        delete m_d3d9Dll;
        m_d3d9Dll = nullptr;
    }
}

void CDx9RhiInstance::GetDeviceInfo(CVector<RhiDeviceInfo_t>& info)
{
	Log_Debug("Enumerating adapters");

    u32 adapterCount = m_d3d9->GetAdapterCount();

	info.Empty();
	info.Reserve(adapterCount);

	for (u32 i = 0; i < adapterCount; i++)
	{
		RhiDeviceInfo_t rhiInfo = {};
		Dx9DeviceInfo_t dxInfo = {};
		if (CDx9RhiDevice::GetDeviceInfo(m_d3d9, rhiInfo, dxInfo, i))
		{
			info.Add(rhiInfo);
			Log_Debug("Device %s [%04x:%04x] is usable", rhiInfo.name.Data(), rhiInfo.vendorId, rhiInfo.deviceId);
		}

		m_devices.Add(dxInfo);
	}
}

IRhiDevice* CDx9RhiInstance::CreateDevice(const RhiDeviceInfo_t& info)
{
	ssize index = static_cast<ssize>(info.handle);
	if (index < 0 || index >= m_devices.Size())
	{
		Log_Error("Device %s has invalid handle %zd", info.name.Data(), index);
		return nullptr;
	}

	Log_Debug("Creating DirectX 9 device for GPU %zd %s [%04x:%04x]", index, info.name.Data(), info.vendorId, info.deviceId);

	CDx9RhiDevice* device = new CDx9RhiDevice(this, m_devices[index]);
	if (!device->Initialize())
	{
		delete device;
		return nullptr;
	}

	return device;
}

#ifdef CH_STATIC
IRhiInstance* CreateDx9RhiInstance()
#else
extern "C" RHIAPI IRhiInstance* CreateInstance()
#endif
{
	return new CDx9RhiInstance();
}
