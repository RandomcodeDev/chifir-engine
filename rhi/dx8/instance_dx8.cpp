/// \file DirectX 8 instance implementation
/// \copyright 2025 Randomcode Developers

#include "base/log.h"

#include "videosystem/ivideosystem.h"

#include "instance_dx8.h"

bool CDx8RhiInstance::LoadDlls()
{
#ifdef CH_WIN32
    Log_Debug("Loading DirectX 8 runtime d3d8.dll");
    m_d3d8Dll = Base_LoadLibrary("d3d8");
    if (!m_d3d8Dll)
    {
        Log_Error("Failed to load D3D8 runtime");
        Destroy();
        return false;
    }
#endif
    return true;
}

bool CDx8RhiInstance::Initialize(IVideoSystem* videoSystem)
{
    m_videoSystem = videoSystem;

    if (!LoadDlls())
    {
        return false;
    }

#ifdef CH_XBOX
    auto f_Direct3DCreate8 = Direct3DCreate8;
#else
    Log_Debug("Getting address of Direct3DCreate8");
    auto f_Direct3DCreate8 = GET_SYMBOL(m_d3d8Dll, Direct3DCreate8);
    if (!f_Direct3DCreate8)
    {
        Log_Error("Failed to get Direct3DCreate8");
        Destroy();
        return false;
    }
#endif

    Log_Debug("Creating IDirect3D8 with SDK version %u", D3D_SDK_VERSION);
    m_d3d8 = f_Direct3DCreate8(D3D_SDK_VERSION);
    if (!m_d3d8)
    {
        Log_Error("Direct3DCreate8 failed");
        Destroy();
        return false;
    }

    return true;
}

void CDx8RhiInstance::Destroy()
{
    if (m_d3d8)
    {
        Log_Debug("Releasing IDirect3D8 0x%016zX", reinterpret_cast<uptr>(m_d3d8));
        m_d3d8->Release();
        m_d3d8 = nullptr;
    }

    if (m_d3d8Dll)
    {
        delete m_d3d8Dll;
        m_d3d8Dll = nullptr;
    }
}

void CDx8RhiInstance::GetDeviceInfo(CVector<RhiDeviceInfo_t>& info)
{
	Log_Debug("Enumerating adapters");

    u32 adapterCount = m_d3d8->GetAdapterCount();

	info.Empty();
	info.Reserve(adapterCount);

	for (u32 i = 0; i < adapterCount; i++)
	{
		RhiDeviceInfo_t rhiInfo = {};
		Dx8DeviceInfo_t dxInfo = {};
		if (CDx8RhiDevice::GetDeviceInfo(m_d3d8, rhiInfo, dxInfo, i))
		{
			info.Add(rhiInfo);
			Log_Debug("Device %s [%04x:%04x] is usable", rhiInfo.name.Data(), rhiInfo.vendorId, rhiInfo.deviceId);
		}

		m_devices.Add(dxInfo);
	}
}

IRhiDevice* CDx8RhiInstance::CreateDevice(const RhiDeviceInfo_t& info)
{
	ssize index = static_cast<ssize>(info.handle);
	if (index < 0 || index >= m_devices.Size())
	{
		Log_Error("Device %s has invalid handle %zd", info.name.Data(), index);
		return nullptr;
	}

	Log_Debug("Creating DirectX 8 device for GPU %zd %s [%04x:%04x]", index, info.name.Data(), info.vendorId, info.deviceId);

	CDx8RhiDevice* device = new CDx8RhiDevice(this, m_devices[index]);
	if (!device->Initialize())
	{
		delete device;
		return nullptr;
	}

	return device;
}

#ifdef CH_STATIC
IRhiInstance* CreateDx8RhiInstance()
#else
extern "C" RHIAPI IRhiInstance* CreateInstance()
#endif
{
	return new CDx8RhiInstance();
}
