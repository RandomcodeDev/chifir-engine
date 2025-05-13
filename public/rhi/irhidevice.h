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
#include "irhiswapchain.h"
#include "rhi.h"

class IRhiPipelineState;
struct RhiPipelineStateDesc_t;

enum class RhiDeviceType_t
{
	Integrated,
	Discrete,
	Software,
	Other
};

struct RhiDeviceInfo_t
{
	CString name;

	RhiDeviceType_t deviceType;

	u32 vendorId;
	u32 deviceId;

	s64 totalMemory;

	u64 handle;
};

/// Represents a VkDevice and a VkQueue, an ID3D12Device and an ID3D12CommandQueue, or similar
class IRhiDevice
{
  public:
	virtual ~IRhiDevice() = default;

	/// Destroy the device
	virtual void Destroy() = 0;

	/// Create a swap chain. The buffer count is only a suggestion, and more or less may be created.
	virtual IRhiSwapChain* CreateSwapChain(u32 bufferCount) = 0;

	/// Create a command list
	virtual IRhiCommandList* CreateCommandList(RhiCommandListFlags_t flags) = 0;

	/// Create a fence
	virtual IRhiFence* CreateFence(u64 initialValue = 0) = 0;

	/// Create a buffer
	virtual IRhiBuffer* CreateBuffer(
		ssize size, RhiMemoryLocation_t location, RhiMemoryUsage_t usage) = 0;

	/// Create an image
	virtual IRhiImage* CreateImage(
		u32 width, u32 height, u32 depth, RhiMemoryLocation_t location, RhiImageType type, RhiImageFormat format) = 0;

	/// Create a pipeline state
	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc) = 0;
};
