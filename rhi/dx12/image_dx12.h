#pragma once

#include "base/types.h"

#include "rhi/irhiimage.h"

#include "dx12.h"

class CDx12RhiRenderTarget: public IRhiRenderTarget
{
  public:
	virtual void Destroy();

	virtual u32 GetWidth();
	virtual u32 GetHeight();
	virtual u32 GetDepth();

  private:
    ID3D12Resource* m_handle;
};
