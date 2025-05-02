/// \file DirectX 12 instance implementation
/// \copyright Randomcode Developers

#include "base/log.h"

#include "videosystem/ivideosystem.h"

#include "device_dx12.h"
#include "instance_dx12.h"

//extern "C" DLLEXPORT u32 D3D12SDKVersion = 615;
//extern "C" DLLEXPORT cstr D3D12SDKPath = "bin\\";

bool CDx12RhiInstance::LoadDlls()
{
	Log_Debug("Loading DXGI %s.dll", DXGI_DLL_NAME);
	m_dxgi = Base_LoadLibrary(DXGI_DLL_NAME);
	if (!m_dxgi)
	{
		Log_Error("Failed to load DXGI");
		Destroy();
		return false;
	}

	// This is when support for the Agility SDK was introduced
	//if (AT_LEAST_WINDOWS_10_BUILD(18363))
	//{
	//	Log_Debug("Loading DirectX 12 Agility SDK");
	//	Base_LoadEngineLibrary("D3D12Core");
	//	Base_LoadEngineLibrary("d3d12SDKLayers");
	//}

	Log_Debug(
		"Loading DirectX 12 runtime %s.dll", D3D12_DLL_NAME);
	m_d3d12 = Base_LoadLibrary(D3D12_DLL_NAME);
	if (!m_d3d12)
	{
		Log_Error("Failed to load D3D12 runtime");
		Destroy();
		return false;
	}

	return true;
}

void CDx12RhiInstance::EnableDebugLayer()
{
	// success of everything here is optional, just don't crash
	Log_Debug("Getting address of D3D12GetDebugInterface");
	auto f_D3D12GetDebugInterface = m_d3d12->GetSymbol<HRESULT(WINAPI*)(REFIID iid, void** debug)>("D3D12GetDebugInterface");
	if (f_D3D12GetDebugInterface)
	{
		ID3D12Debug6* debug;
		HRESULT result = f_D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
		if (SUCCEEDED(result))
		{
			debug->EnableDebugLayer();
			debug->SetEnableAutoName(true);
			debug->Release();

			Log_Debug("DirectX 12 debug layer enabled");
		}
		else
		{
			Log_Error("D3D12GetDebugInterface failed: HRESULT 0x%08x", result);
		}
	}
	else
	{
		Log_Error("Failed to get address of D3D12GetDebugInterface!");
	}
}

bool CDx12RhiInstance::Initialize(IVideoSystem* videoSystem)
{
	m_videoSystem = videoSystem;

	if (!LoadDlls())
	{
		return false;
	}

	Log_Debug("Getting address of CreateDXGIFactory2");
	auto f_CreateDXGIFactory2 = m_dxgi->GetSymbol<decltype(CreateDXGIFactory2)*>("CreateDXGIFactory2");
	if (!f_CreateDXGIFactory2)
	{
		Log_Error("Failed to get CreateDXGIFactory2");
		Destroy();
		return false;
	}

#ifdef CH_DEBUG
	u32 factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
	u32 factoryFlags = 0;
#endif
	Log_Debug("Creating IDXGIFactory6%s", factoryFlags == DXGI_CREATE_FACTORY_DEBUG ? " with DXGI_CREATE_FACTORY_DEBUG" : "");
	HRESULT result = f_CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory));
	if (FAILED(result))
	{
		Log_Error("CreateDXGIFactory2 failed: HRESULT 0x%08X", result);
		LastNtError() = result;
		LastNtStatus() = 0;
		Destroy();
		return false;
	}

#ifdef CH_DEBUG
	EnableDebugLayer();
#endif

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
	while (SUCCEEDED(m_factory->EnumAdapterByGpuPreference(i++, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter))))
	{
		adapters.Add(adapter);
	}

	info.Empty();
	info.Reserve(adapters.Size());

	for (ssize i = 0; i < adapters.Size(); i++)
	{
		RhiDeviceInfo_t rhiInfo = {};
		Dx12DeviceInfo_t dxInfo = {};
		if (CDx12RhiDevice::GetDeviceInfo(rhiInfo, dxInfo, adapters[i], i))
		{
			info.Add(rhiInfo);
			Log_Debug("Device %s [%04x:%04x] is usable", rhiInfo.name.Data(), rhiInfo.vendorId, rhiInfo.deviceId);
		}

		m_devices.Add(dxInfo);
	}
}

IRhiDevice* CDx12RhiInstance::CreateDevice(const RhiDeviceInfo_t& info)
{
	ssize index = static_cast<ssize>(info.handle);
	if (index < 0 || index >= m_devices.Size())
	{
		Log_Error("Device %s has invalid handle %zd", info.name.Data(), index);
		return nullptr;
	}

	Log_Debug("Creating DirectX 12 device for GPU %zd %s [%04x:%04x]", index, info.name.Data(), info.vendorId, info.deviceId);

	CDx12RhiDevice* device = new CDx12RhiDevice(this, m_devices[index]);
	if (!device->Initialize())
	{
		delete device;
		return nullptr;
	}

	return device;
}

#ifdef CH_STATIC
IRhiInstance* CreateDx12RhiInstance()
#else
extern "C" RHIAPI IRhiInstance* CreateInstance()
#endif
{
	return new CDx12RhiInstance();
}
