/// \file DirectX 9 image classes
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "rhi/irhiimage.h"

#include "dx9.h"

class CDx9RhiDevice;

class CDx9RhiImage: public IRhiImage
{
  public:
	// 1D or cube
	CDx9RhiImage(
		CDx9RhiDevice* device, u32 size, u32 mipLevels, RhiMemoryLocation location, RhiImageFormat format, RhiImageType type,
		RhiImageUsage usage);
	// 2D
	CDx9RhiImage(
		CDx9RhiDevice* device, u32 width, u32 height, u32 mipLevels, RhiMemoryLocation location, RhiImageFormat format,
		RhiImageUsage usage);
	~CDx9RhiImage() = default;

  private:
	friend class CDx9RhiDevice;

	CDx9RhiDevice* m_device;
	IDirect3DTexture9* m_handle;
    RhiMemoryLocation m_location;
    RhiImageType m_type;
	u32 m_width;
	u32 m_height;
    u32 m_depth;
};
