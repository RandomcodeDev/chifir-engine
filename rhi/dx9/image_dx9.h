/// \file DirectX 9 image classes
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "rhi/irhiimage.h"

#include "dx9.h"

class CDx9RhiDevice;

class CDx9RhiImage: public IRhiImage, CDx9RhiBaseObject<IDirect3DTexture9>
{
  public:
	~CDx9RhiImage() = default;

	virtual void Destroy()
	{
		CDx9RhiBaseObject::Destroy();
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
	friend class CDx9RhiDevice;

	RhiMemoryLocation m_location;
	RhiImageFormat m_format;
	RhiImageType m_type;
	RhiImageUsage m_usage;
	u32 m_width;
	u32 m_height;
	u32 m_depth;
	u32 m_mipLevels;

	// 1D or cube
	CDx9RhiImage(
		CDx9RhiDevice* device, u32 size, u32 mipLevels, RhiMemoryLocation location, RhiImageFormat format, RhiImageType type,
		RhiImageUsage usage)
		: CDx9RhiBaseObject(device), m_location(location), m_format(format), m_type(type), m_usage(usage), m_width(size),
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
	CDx9RhiImage(
		CDx9RhiDevice* device, u32 width, u32 height, u32 mipLevels, RhiMemoryLocation location, RhiImageFormat format,
		RhiImageUsage usage)
		: CDx9RhiBaseObject(device), m_location(location), m_format(format), m_type(RhiImageType::Image2d), m_usage(usage),
		  m_width(width), m_height(height), m_depth(1), m_mipLevels(mipLevels)
	{
	}

	bool Initialize();
};

class CDx9RhiImageView: public IRhiImageView
{};

class CDx9RhiRenderTarget: public IRhiRenderTarget, CDx9RhiBaseObject<IDirect3DSurface9>
{
  public:
	~CDx9RhiRenderTarget() = default;

	virtual void Destroy()
	{
		CDx9RhiBaseObject::Destroy();
	}

	virtual IDirect3DSurface9* GetHandle()
	{
		return CDx9RhiBaseObject::GetHandle();
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
	friend class CDx9RhiDevice;
	friend class CDx9RhiSwapChain;

	CDx9RhiImageView* m_view;
	RhiMemoryLocation m_location;
	RhiImageFormat m_format;
	RhiImageType m_type;
	RhiImageUsage m_usage;
	u32 m_width;
	u32 m_height;
	u32 m_depth;
	u32 m_mipLevels;

	CDx9RhiRenderTarget(CDx9RhiDevice* device, CDx9RhiImageView* view);
	CDx9RhiRenderTarget(CDx9RhiDevice* device, IDirect3DSurface9* surface);
};
