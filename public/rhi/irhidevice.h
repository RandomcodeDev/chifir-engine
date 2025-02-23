/// RHI device interface

#pragma once

#include "base/compiler.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"

#include "rhi.h"

class IRhiSwapChain;
class IRhiCommandList;
enum class RhiCommandListFlags_t;
class IRhiFence;
enum class RhiMemoryLocation_t;
enum class RhiMemoryUsage_t;
class IRhiBuffer;
enum class RhiBufferFlags_t;
class IRhiImage;
enum class RhiImageType_t;
enum class RhiImageFormat_t;
enum class RhiImageFlags_t;
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
	s32 maxTextureSize;

	u64 handle;
};

// VkDevice + VkQueue, ID3D12Device + ID3D12CommandQueue
class IRhiDevice
{
  public:
	virtual ~IRhiDevice() DEFAULT;

	/// Destroy the device
	virtual void Destroy() = 0;

	/// Create a swap chain
	virtual IRhiSwapChain* CreateSwapChain(CVector<IRhiImage*>& buffers) = 0;

	/// Create a command list
	virtual IRhiCommandList* CreateCommandList(RhiCommandListFlags_t flags) = 0;

	/// Create a fence
	virtual IRhiFence* CreateFence(u64 initialValue = 0) = 0;

	/// Create a buffer
	virtual IRhiBuffer* CreateBuffer(
		ssize size, RhiMemoryLocation_t location, RhiMemoryUsage_t usage, RhiBufferFlags_t flags) = 0;

	/// Create an image
	virtual IRhiImage* CreateImage(
		u32 width, u32 height, u32 depth, RhiMemoryLocation_t location, RhiImageType_t type, RhiImageFormat_t format,
		RhiImageFlags_t flags) = 0;

	/// Create a pipeline state
	virtual IRhiPipelineState* CreatePipelineState(const RhiPipelineStateDesc_t& desc) = 0;
};
