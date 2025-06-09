/// \file Legacy DirectX helper stuff
/// \copyright 2025 Randomcode Developers

#include "rhi/irhibuffer.h"
#include "rhi/irhiimage.h"

#include "dxcommon.h"

#if defined IN_RHI_DIRECTX8 || defined IN_RHI_DIRECTX9
D3DPOOL GetPoolForMemoryLocation(RhiMemoryLocation location)
{
    switch (location)
    {
    case RhiMemoryLocation::Host:
        return D3DPOOL_SYSTEMMEM;
    case RhiMemoryLocation::Shared:
        return D3DPOOL_MANAGED;
    case RhiMemoryLocation::Device:
    default:
        return D3DPOOL_DEFAULT;
    }
}

DWORD GetUsageForImageUsage(RhiImageUsage usage)
{
    switch (usage)
    {
    case RhiImageUsage::ColorTarget:
        return D3DUSAGE_RENDERTARGET;
    case RhiImageUsage::DepthStencilTarget:
        return D3DUSAGE_DEPTHSTENCIL;
    case RhiImageUsage::Texture:
    default:
        return 0;
    }
}

D3DFORMAT GetDx9Format(RhiImageFormat format)
{
    switch (format)
    {
    case RhiImageFormat::Rgba32Uint:
    case RhiImageFormat::Rgba32Unorm:
        return D3DFMT_A8R8G8B8; // swizzle in shaders
    case RhiImageFormat::D24S8Float:
        return D3DFMT_D24S8;
    default:
        return D3DFMT_UNKNOWN;
    }
}

RhiImageFormat GetRhiFormat(D3DFORMAT format)
{
    switch (format)
    {
    case D3DFMT_A8R8G8B8:
        return RhiImageFormat::Rgba32Uint;
    case D3DFMT_D24S8:
        return RhiImageFormat::D24S8Float;
    default:
        return RhiImageFormat::Unknown;
    }
}
#endif
