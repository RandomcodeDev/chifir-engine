/// \file DirectX 9 image/render target stuff
/// \copyright 2025 Randomcode Developers

#include "dx9.h"
#include "device_dx9.h"
#include "image_dx9.h"

bool CDx9RhiImage::Initialize()
{
	return false;
}

CDx9RhiRenderTarget::CDx9RhiRenderTarget(CDx9RhiDevice* device, CDx9RhiImageView* view) : CDxRhiBaseObject(device), m_view(view)
{
}

CDx9RhiRenderTarget::CDx9RhiRenderTarget(CDx9RhiDevice* device, IDirect3DSurface9* surface) : CDxRhiBaseObject(device, surface), m_view(nullptr)
{
	D3DSURFACE_DESC desc = {};
	m_handle->GetDesc(&desc);
	m_width = desc.Width;
	m_height = desc.Height;
}
