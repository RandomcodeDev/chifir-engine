#include "base/basicstr.h"
#include "base/log.h"
#include "base/platform.h"

#include "rhi/irhidevice.h"

#include "dx12.h"
#include "device_dx12.h"
#include "instance_dx12.h"
#include "swapchain_dx12.h"

CDx12RhiDevice::CDx12RhiDevice(CDx12RhiInstance* instance, const Dx12DeviceInfo_t& info) : m_instance(instance), m_info(info)
{
}

bool CDx12RhiDevice::Initialize()
{
	Log_Debug("Getting address of D3D12CreateDevice in %s.dll", CDx12RhiInstance::D3D12_DLL_NAME);
	auto f_D3D12CreateDevice =
		m_instance->m_d3d12->GetSymbol<HRESULT(WINAPI*)(IUnknown * adapter, D3D_FEATURE_LEVEL, const GUID iid, void** device)>(
			"D3D12CreateDevice");
	if (!f_D3D12CreateDevice)
	{
		Log_Error("Failed to get address of D3D12CreateDevice!");
		return false;
	}

	Log_Debug("Creating ID3D12Device4 with feature level " STRINGIZE_EXPAND(DX12_TARGET_FEATURE_LEVEL));
	HRESULT result = f_D3D12CreateDevice(m_info.adapter, DX12_TARGET_FEATURE_LEVEL, IID_PPV_ARGS(&m_handle));
	if (!SUCCEEDED(result))
	{
		Log_Error("D3D12CreateDevice failed: HRESULT 0x%08X", result);
		return false;
	}

	Log_Debug("Creating command queues");
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	result = m_handle->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_queue));
	if (!SUCCEEDED(result))
	{
		Log_Error("Failed to create direct command queue: HRESULT 0x%08X", result);
		Destroy();
		return false;
	}

	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	result = m_handle->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_copyQueue));
	if (!SUCCEEDED(result))
	{
		Log_Error("Failed to create copy command queue: HRESULT 0x%08X", result);
		Destroy();
		return false;
	}

	return true;
}

void CDx12RhiDevice::Destroy()
{
	if (m_copyQueue)
	{
		Log_Debug("Releasing copy queue 0x%016X", m_copyQueue);
		m_copyQueue->Release();
		m_copyQueue = nullptr;
	}

	if (m_queue)
	{
		Log_Debug("Releasing direct queue 0x%016X", m_queue);
		m_queue->Release();
		m_queue = nullptr;
	}

	if (m_handle)
	{
		Log_Debug("Releasing ID3D12Device4 0x%016X", m_handle);
		m_handle->Release();
		m_handle = nullptr;
	}
}

IRhiSwapChain* CDx12RhiDevice::CreateSwapChain(u32 bufferCount)
{
	CDx12RhiSwapChain* swapChain = new CDx12RhiSwapChain(this);
	if (!swapChain->Initialize(bufferCount))
	{
		delete swapChain;
		return nullptr;
	}

	return swapChain;
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
