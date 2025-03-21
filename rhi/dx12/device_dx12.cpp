#include "base/basicstr.h"
#include "base/log.h"
#include "base/platform.h"
#include "phnt.h"
#include "rhi/irhidevice.h"

#include "device_dx12.h"
#include "instance_dx12.h"


CDx12RhiDevice::CDx12RhiDevice(CDx12RhiInstance* instance, const Dx12DeviceInfo_t& info) : m_instance(instance), m_info(info)
{
}

bool CDx12RhiDevice::Initialize()
{
	Log_Debug("Getting address of D3D12CreateDevice in %s.dll", CDx12RhiInstance::D3D12_DLL_NAME);
	auto f_D3D12CreateDevice =
		m_instance->GetD3d12Symbol<HRESULT(WINAPI*)(IUnknown * adapter, D3D_FEATURE_LEVEL, const GUID iid, void** device)>(
			"D3D12CreateDevice");
	if (!f_D3D12CreateDevice)
	{
		Log_Error("Failed to get address of D3D12CreateDevice!");
		return false;
	}

	Log_Debug("Creating device with feature level " STRINGIZE_EXPAND(DX12_TARGET_FEATURE_LEVEL));
	HRESULT result = f_D3D12CreateDevice(m_info.adapter, DX12_TARGET_FEATURE_LEVEL, IID_PPV_ARGS(&m_handle));
	if (!SUCCEEDED(result))
	{
		Log_Error("D3D12CreateDevice failed: HRESULT 0x%08X", result);
		return false;
	}

	return true;
}

void CDx12RhiDevice::Destroy()
{
}

IRhiSwapChain* CDx12RhiDevice::CreateSwapChain(u32 bufferCount, CVector<IRhiRenderTarget*>& buffers)
{
	UNUSED(bufferCount);
	UNUSED(buffers);
	return nullptr;
}

bool CDx12RhiDevice::GetDeviceInfo(RhiDeviceInfo_t& rhiInfo, Dx12DeviceInfo_t& info, IDXGIAdapter1* adapter, ssize i)
{
	Log_Debug("Getting information for adapter %zd", i);

	info.adapter = adapter;
	HRESULT result = adapter->GetDesc1(&info.desc);
	if (!SUCCEEDED(result))
	{
		Log_Error("Failed to get adapter description: HRESULT 0x%08X", result);
		return false;
	}

	// convert the description to ASCII
	UNICODE_STRING descWStr = {};
	descWStr.Buffer = info.desc.Description;
	descWStr.MaximumLength = sizeof(info.desc.Description);
	descWStr.Length = static_cast<u16>(Base_MemFind(descWStr.Buffer, descWStr.MaximumLength, L"", sizeof(wchar_t)) + 1);
	ANSI_STRING descStr = {};
	RtlUnicodeStringToAnsiString(&descStr, &descWStr, TRUE);
	rhiInfo.name = CString(descStr.Buffer, descStr.Length);
	RtlFreeAnsiString(&descStr);

	rhiInfo.handle = i;

	// this basically only matters in vulkan, but still fill it out
	rhiInfo.deviceType = (info.desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) ? RhiDeviceType_t::Software : RhiDeviceType_t::Discrete;

	rhiInfo.vendorId = info.desc.VendorId;
	rhiInfo.deviceId = info.desc.DeviceId;

	// for igpus/xbox the shared memory matters
	rhiInfo.totalMemory = info.desc.DedicatedVideoMemory + info.desc.SharedSystemMemory;

	return true;
}
