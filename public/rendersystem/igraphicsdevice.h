// Graphics device interface

#pragma once

#include "base/compiler.h"
#include "base/types.h"

class IGraphicsCommandQueue;
enum GraphicsCommandQueueType_t;
enum GraphicsCommandQueueFlags_t;
class IGraphicsCommandList;
enum GraphicsCommandListFlags_t;
class IGraphicsFence;
enum GraphicsMemoryLocation_t;
enum GraphicsMemoryUsage_t;
class IGraphicsBuffer;
enum GraphicsBufferFlags_t;
class IGraphicsImage;
enum GraphicsImageType_t;
enum GraphicsImageFormat_t;
enum GraphicsImageFlags_t;

class IGraphicsDevice
{
  public:
	virtual ~IGraphicsDevice() DEFAULT;

	// Create a command queue
	virtual IGraphicsCommandQueue* CreateCommandQueue(GraphicsCommandQueueType_t type, GraphicsCommandQueueFlags_t flags) = 0;

	// Create a command list
	virtual IGraphicsCommandList* CreateCommandList(IGraphicsCommandQueue* queue, GraphicsCommandListFlags_t flags) = 0;

	// Create a fence
	virtual IGraphicsFence* CreateFence(u64 initialValue = 0) = 0;

	// Create a buffer
	virtual IGraphicsBuffer* CreateBuffer(
		ssize size, GraphicsMemoryLocation_t location, GraphicsMemoryUsage_t usage, GraphicsBufferFlags_t flags) = 0;

	// Create an image
	virtual IGraphicsImage* CreateImage(
		u32 width, u32 height, u32 depth, GraphicsMemoryLocation_t location, GraphicsImageType_t type,
		GraphicsImageFormat_t format, GraphicsImageFlags_t flags) = 0;
};
