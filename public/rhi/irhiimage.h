#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "rhi/irhibuffer.h"

/// The number of dimensions in an image
enum class RhiImageType_t
{
    Image1d,
    Image2d,
    Image3d
};

/// The format of an image
/// All color formats are sRGB
enum class RhiImageFormat_t
{
    R8, // 8-bit color
    Rg16, // 16-bit color
    Rgb24, // 24-bit color
    Rgba32, // 32-bit color
    D24, // 24-bit depth
};

/// Represents a VkImage or an ID3D12Resource used as an image
class IRhiImage
{
  public:
    virtual ~IRhiImage() DEFAULT;

    virtual void Destroy() = 0;

    virtual RhiImageType_t GetType() = 0;
    virtual RhiImageFormat_t GetFormat() = 0;

    virtual u32 GetWidth() = 0;
    virtual u32 GetHeight() = 0;
    virtual u32 GetDepth() = 0;
};

/// A view of an image (VkImageView or a shader resource view)
class IRhiImageView
{
  public:
    virtual ~IRhiImageView() DEFAULT;

    // Get the image this is a view of, if it's controlled by the RHI
    virtual IRhiImage* GetImage() = 0;

    virtual void Destroy() = 0;

    virtual u32 GetWidth() = 0;
    virtual u32 GetHeight() = 0;
    virtual u32 GetDepth() = 0;
};

/// A render target
class IRhiRenderTarget: public IRhiImageView
{
  public:
    virtual ~IRhiRenderTarget() DEFAULT;

    virtual void Destroy() = 0;

    virtual u32 GetWidth() = 0;
    virtual u32 GetHeight() = 0;
    virtual u32 GetDepth() = 0;

    // TODO: add other stuff
};
