#include "base/log.h"

#include "videosystem/ivideosystem.h"

#include "instance_dx12.h"

bool CDx12RhiInstance::Initialize(IVideoSystem* videoSystem)
{
    m_hwnd = reinterpret_cast<HWND>(videoSystem->GetHandle());

    Log_Debug("Creating IDXGIFactory6");
    HRESULT result = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory));
    if (!HR_SUCCESS(result))
    {
        Log_Error("CreateDXGIFactory2 failed: HRESULT 0x%08X", result);
        return false;
    }

    return true;
}

void CDx12RhiInstance::Destroy()
{

}

void CDx12RhiInstance::GetDeviceInfo(CVector<RhiDeviceInfo_t>& info)
{
    UNUSED(info);
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
