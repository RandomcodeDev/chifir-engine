/// \file RHI command list interfaces
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "rhi.h"

class IRhiRenderTarget;
class IRhiDepthStencilTarget;

class IRhiCommandList: public IRhiBaseObject
{
  public:
	virtual ~IRhiCommandList() = default;

	virtual void Destroy() = 0;

	/// Lock the command buffer for recording on this thread
	virtual void BeginCommands() = 0;

	/// Unlock the command buffer
	virtual void EndCommands() = 0;

	virtual void SetRenderTarget(u32 index, IRhiRenderTarget* target) = 0;
	virtual void SetDepthStencilTarget(IRhiDepthStencilTarget* target) = 0;

	// TODO: vector4 overload?
	virtual void Clear(u32 color = 0, f32 depth = 0.0, u8 stencil = 1) = 0;
};

enum class RhiCommandListFlags
{
};

class IRhiFence
{
};
