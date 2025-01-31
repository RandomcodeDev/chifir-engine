/// RHI device interface

#pragma once

#include "base/compiler.h"
#include "base/string.h"
#include "base/types.h"

#include "rhi.h"

class IRhiCommandQueue;
enum class RhiCommandQueueType_t;
enum class RhiCommandQueueFlags_t;
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

	~RhiDeviceInfo_t()
	{
		if (freeHandle && handle)
		{
			Base_Free(reinterpret_cast<void*>(handle));
		}
	}

private:
	friend class CVulkanRhiDevice;

	bool freeHandle;
};

class RHIAPI IRhiDevice
{
  public:
	virtual ~IRhiDevice() DEFAULT;

	/// Create a command queue
	virtual IRhiCommandQueue* CreateCommandQueue(RhiCommandQueueType_t type, RhiCommandQueueFlags_t flags) = 0;

	/// Create a command list
	virtual IRhiCommandList* CreateCommandList(IRhiCommandQueue* queue, RhiCommandListFlags_t flags) = 0;

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
