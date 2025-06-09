/// \file DirectX 8 image classes
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "rhi/irhiimage.h"

#include "dx8.h"

class CDx8RhiDevice;

class CDx8RhiImage: public IRhiImage, public CDxRhiBaseObject<IDirect3DTexture8>
{
  public:
	~CDx8RhiImage() = default;

	virtual void Destroy()
	{
		CDxRhiBaseObject::Destroy();
	}

	virtual RhiImageType GetType()
	{
		return m_type;
	}

	virtual RhiImageFormat GetFormat()
	{
		return m_format;
	}

	virtual RhiImageUsage GetUsage()
	{
		return m_usage;
	}

	virtual u32 GetWidth()
	{
		return m_width;
	}
	virtual u32 GetHeight()
	{
		return m_height;
	}

	virtual u32 GetDepth()
	{
		return m_depth;
	}

	virtual u32 GetMipLevels()
	{
		return m_mipLevels;
	}

  private:
	friend class CDx8RhiDevice;

	RhiMemoryLocation m_location;
	RhiImageFormat m_format;
	RhiImageType m_type;
	RhiImageUsage m_usage;
	u32 m_width;
	u32 m_height;
	u32 m_depth;
	u32 m_mipLevels;

	// 1D or cube
	CDx8RhiImage(
		CDx8RhiDevice* device, u32 size, u32 mipLevels, RhiMemoryLocation location, RhiImageFormat format, RhiImageType type,
		RhiImageUsage usage)
		: CDxRhiBaseObject(device), m_location(location), m_format(format), m_type(type), m_usage(usage), m_width(size),
		  m_height(1), m_depth(1), m_mipLevels(mipLevels)
	{
		if (m_type == RhiImageType::Image3d)
		{
			// only cube textures
			m_height = size;
			m_depth = size;
		}
	}

	// 2D
	CDx8RhiImage(
		CDx8RhiDevice* device, u32 width, u32 height, u32 mipLevels, RhiMemoryLocation location, RhiImageFormat format,
		RhiImageUsage usage)
		: CDxRhiBaseObject(device), m_location(location), m_format(format), m_type(RhiImageType::Image2d), m_usage(usage),
		  m_width(width), m_height(height), m_depth(1), m_mipLevels(mipLevels)
	{
	}

	bool Initialize();
};

class CDx8RhiImageView: public IRhiImageView
{};

class CDx8RhiRenderTarget: public IRhiRenderTarget, public CDxRhiBaseObject<IDirect3DSurface8>
{
  public:
	~CDx8RhiRenderTarget() = default;

	virtual void Destroy()
	{
		CDxRhiBaseObject::Destroy();
	}

	virtual IDirect3DSurface8* GetHandle()
	{
		return CDxRhiBaseObject::GetHandle();
	}

	virtual IRhiImage* GetImage()
	{
		if (m_view)
		{
			return m_view->GetImage();
		}

		return nullptr;
	}

	virtual RhiImageType GetType()
	{
		return m_type;
	}

	virtual RhiImageFormat GetFormat()
	{
		return m_format;
	}

	virtual RhiImageUsage GetUsage()
	{
		return m_usage;
	}

	virtual u32 GetWidth()
	{
		return m_width;
	}
	virtual u32 GetHeight()
	{
		return m_height;
	}

	virtual u32 GetDepth()
	{
		return m_depth;
	}

	virtual u32 GetMipLevels()
	{
		return m_mipLevels;
	}

  private:
	friend class CDx8RhiDevice;
	friend class CDx8RhiSwapChain;

	CDx8RhiImageView* m_view;
	RhiMemoryLocation m_location;
	RhiImageFormat m_format;
	RhiImageType m_type;
	RhiImageUsage m_usage;
	u32 m_width;
	u32 m_height;
	u32 m_depth;
	u32 m_mipLevels;

	CDx8RhiRenderTarget(CDx8RhiDevice* device, CDx8RhiImageView* view);
	CDx8RhiRenderTarget(CDx8RhiDevice* device, IDirect3DSurface8* surface);
};
