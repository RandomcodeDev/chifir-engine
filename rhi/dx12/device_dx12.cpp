#include "base/basicstr.h"
#include "base/log.h"
#include "base/platform.h"
#include "phnt.h"

#include "device_dx12.h"

CDx12RhiDevice::CDx12RhiDevice(CDx12RhiInstance* instance, const Dx12DeviceInfo_t& info) : m_instance(instance), m_info(info)
{
}

bool CDx12RhiDevice::Initialize()
{
	return false;
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
	descWStr.Length = static_cast<u16>(Base_MemFind(descWStr.Buffer, descWStr.MaximumLength, L"\0", sizeof(wchar_t)) - 1);
	ANSI_STRING descStr = {};
	RtlUnicodeStringToAnsiString(&descStr, &descWStr, TRUE);
	rhiInfo.name = CString(descStr.Buffer, descStr.Length);
	RtlFreeAnsiString(&descStr);

	rhiInfo.handle = i;

	rhiInfo.vendorId = info.desc.VendorId;
	rhiInfo.deviceId = info.desc.DeviceId;

	rhiInfo.totalMemory = info.desc.DedicatedVideoMemory + info.desc.SharedSystemMemory;

	return true;
}
