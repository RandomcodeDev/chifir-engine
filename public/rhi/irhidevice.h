/// \file RHI device interface
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"

#include "irhibuffer.h"
#include "irhicommandlist.h"
#include "irhiimage.h"
#include "irhimemory.h"
#include "irhiswapchain.h"
#include "rhi.h"

class IRhiPipelineState;
struct RhiPipelineStateDesc_t;

enum class RhiDeviceType
{
	Integrated,
	Discrete,
	Software,
	Other
};

struct RhiDeviceInfo_t
{
	CString name;

	RhiDeviceType deviceType;

	u32 vendorId;
	u32 deviceId;

	s64 totalMemory;

	u64 handle;
};

/// A device and its queues
class IRhiDevice
{
  public:
	virtual ~IRhiDevice() = default;

	/// Destroy the device
	virtual void Destroy() = 0;

	/// Create a swap chain. The buffer count is only a suggestion, and more or less may be created.
	virtual IRhiSwapChain* CreateSwapChain(u32 bufferCount) = 0;

	/// Create a command list
	virtual IRhiCommandList* CreateCommandList(RhiCommandListFlags flags, ssize bufferSize = 4 * 1024) = 0;

	/// Create a fence
	virtual IRhiFence* CreateFence(u64 initialValue = 0) = 0;

	/// Create a buffer
	virtual IRhiBuffer* CreateBuffer(
		ssize size, RhiMemoryLocation location, RhiBufferUsage usage) = 0;

	/// Create a 2D image
	virtual IRhiImage* CreateImage2d(
		u32 width, u32 height, u32 mipLevels, RhiMemoryLocation location, RhiImageType type, RhiImageFormat format, RhiImageUsage usage) = 0;

    /// Create an image view
    virtual IRhiImageView* CreateImageView(IRhiImage* image) = 0;

    /// Create a render target
    virtual IRhiRenderTarget* CreateRenderTarget(IRhiImageView* imageView) = 0;

	/// Create a pipeline state
	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc) = 0;

	/// Execute a command list
	virtual void ExecuteCommandList(IRhiCommandList* cmdList) = 0;
};
