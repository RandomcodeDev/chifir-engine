/// \file RHI image interface
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "irhibuffer.h"
#include "rhi.h"

/// The number of dimensions in an image
enum class RhiImageType
{
	Image1d,
	Image2d,
    Image3d
};

/// The format of an image
/// All color formats are sRGB
enum class RhiImageFormat
{
	R8Uint,      // 8-bit color, uint
	Rg16Uint,    // 16-bit color, uint
	Rgb24Uint,   // 24-bit color, uint
	Rgba32Uint,  // 32-bit color, uint
	R8Unorm,     // 8-bit color, uint
	Rg16Unorm,   // 16-bit color, uint normalized
	Rgb24Unorm,  // 24-bit color, uint normalized
	Rgba32Unorm, // 32-bit color, uint normalized
	D24Float,    // 24-bit depth, float
};

/// Image usage
enum class RhiImageUsage
{
    Texture = 1 << 0,
    ColorTarget = 1 << 1,
    DepthStencilTarget = 1 << 2,
};

/// Represents a VkImage or an ID3D12Resource used as an image
class IRhiImage: public IRhiBaseObject
{
  public:
	virtual ~IRhiImage() = default;

	virtual void Destroy() = 0;

	virtual RhiImageType GetType() = 0;
	virtual RhiImageFormat GetFormat() = 0;
    virtual RhiImageUsage GetUsage() = 0;

	virtual u32 GetWidth() = 0;
	virtual u32 GetHeight() = 0;
	virtual u32 GetDepth() = 0;

	virtual u32 GetMipLevels() = 0;
};

/// A view of an image (VkImageView or a shader resource view)
class IRhiImageView: public IRhiBaseObject
{
  public:
	virtual ~IRhiImageView() = default;

	virtual void Destroy() = 0;

	// Get the image this is a view of, if it's controlled by the RHI
	virtual IRhiImage* GetImage() = 0;

	virtual RhiImageType GetType() = 0;
	virtual RhiImageFormat GetFormat() = 0;
    virtual RhiImageUsage GetUsage() = 0;

	virtual u32 GetWidth() = 0;
	virtual u32 GetHeight() = 0;
	virtual u32 GetDepth() = 0;
};

/// A render target
class IRhiRenderTarget: public IRhiImageView
{
  public:
	virtual ~IRhiRenderTarget() = default;

	virtual void Destroy() = 0;

	virtual IRhiImage* GetImage() = 0;

	virtual RhiImageType GetType() = 0;
	virtual RhiImageFormat GetFormat() = 0;

	virtual u32 GetWidth() = 0;
	virtual u32 GetHeight() = 0;
	virtual u32 GetDepth() = 0;

	// TODO: add other stuff
};

/// A depth/stencil target
class IRhiDepthStencilTarget: public IRhiImageView
{
  public:
	virtual ~IRhiDepthStencilTarget() = default;

	virtual void Destroy() = 0;

	virtual IRhiImage* GetImage() = 0;

	virtual RhiImageType GetType() = 0;
	virtual RhiImageFormat GetFormat() = 0;

	virtual u32 GetWidth() = 0;
	virtual u32 GetHeight() = 0;
};
