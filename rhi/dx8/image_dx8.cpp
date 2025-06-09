/// \file DirectX 8 image/render target stuff
/// \copyright 2025 Randomcode Developers

#include "dx8.h"
#include "device_dx8.h"
#include "image_dx8.h"

bool CDx8RhiImage::Initialize()
{
	return false;
}

CDx8RhiRenderTarget::CDx8RhiRenderTarget(CDx8RhiDevice* device, CDx8RhiImageView* view) : CDxRhiBaseObject(device), m_view(view)
{
}

CDx8RhiRenderTarget::CDx8RhiRenderTarget(CDx8RhiDevice* device, IDirect3DSurface8* surface) : CDxRhiBaseObject(device, surface), m_view(nullptr)
{
	D3DSURFACE_DESC desc = {};
	m_handle->GetDesc(&desc);
	m_width = desc.Width;
	m_height = desc.Height;
}
