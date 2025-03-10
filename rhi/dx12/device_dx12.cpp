#include "base/log.h"

#include "device_dx12.h"

CDx12RhiDevice::CDx12RhiDevice(CDx12RhiInstance* instance, const Dx12DeviceInfo_t& info) : m_instance(instance), m_info(info)
{
}

bool CDx12RhiDevice::Initialize()
{
    return false;
}
