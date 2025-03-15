#include "base/loader.h"
#include "base/log.h"

#include "videosystem/ivideosystem.h"

#include "device_dx12.h"
#include "instance_dx12.h"

extern "C" DLLEXPORT u32 D3D12SDKVersion = 4;
extern "C" DLLEXPORT cstr D3D12SDKPath = ".\\bin\\";

bool CDx12RhiInstance::Initialize(IVideoSystem* videoSystem)
{
	Log_Debug("Loading DXGI");
	m_dxgi = Base_LoadLibrary("dxgi");
	if (!m_dxgi)
	{
		Log_Error("Failed to load DXGI");
		Destroy();
		return false;
	}
	
    Log_Debug("Loading DirectX 12 runtime");
    m_d3d12 = Base_LoadLibrary("d3d12");
	if (!m_d3d12)
	{
		Log_Error("Failed to load D3D12 runtime");
		Destroy();
		return false;
	}

	Log_Debug("Getting address of CreateDXGIFactory2");
	auto s_CreateDXGIFactory2 = m_dxgi->GetSymbol<HRESULT (*)(UINT, const IID& iid, void** factory)>("CreateDXGIFactory2");
	if (!s_CreateDXGIFactory2)
	{
		Log_Error("Failed to get CreateDXGIFactory2");
		Destroy();
		return false;
	}

	m_hwnd = reinterpret_cast<HWND>(videoSystem->GetHandle());

	Log_Debug("Creating IDXGIFactory6");
	HRESULT result = s_CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory));
	if (!HR_SUCCESS(result))
	{
		Log_Error("CreateDXGIFactory2 failed: HRESULT 0x%08X", result);
		Destroy();
		return false;
	}

	return true;
}

void CDx12RhiInstance::Destroy()
{
	if (m_factory)
	{
		Log_Debug("Releasing IDXGIFactory6 0x%016X", reinterpret_cast<uptr>(m_factory));
		m_factory->Release();
		m_factory = nullptr;
	}
	if (m_d3d12)
	{
		delete m_d3d12;
	}
	if (m_dxgi)
	{
		delete m_dxgi;
	}
}

void CDx12RhiInstance::GetDeviceInfo(CVector<RhiDeviceInfo_t>& info)
{
	Log_Debug("Enumerating adapters");

	u32 i = 0;
	IDXGIAdapter1* adapter;
    CVector<IDXGIAdapter1*> adapters;
	while (HR_SUCCESS(m_factory->EnumAdapterByGpuPreference(i++, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter))))
	{
        adapters.Add(adapter);
	}

    info.Empty();
    info.Reserve(adapters.Size());

    for (ssize i = 0; i < info.Size(); i++)
    {
//        RhiDeviceInfo_t rhiInfo = {};
//        Dx12DeviceInfo_t dxInfo = {};
    }
}

IRhiDevice* CDx12RhiInstance::CreateDevice(const RhiDeviceInfo_t& info)
{
	UNUSED(info);
	return nullptr;
}

#ifdef CH_STATIC
IRhiInstance* CreateDx12RhiInstance()
#else
extern "C" RHIAPI IRhiInstance* CreateInstance()
#endif
{
	return new CDx12RhiInstance();
}
