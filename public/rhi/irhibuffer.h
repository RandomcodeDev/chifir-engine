/// \file RHI memory buffers
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "irhimemory.h"

/// Represents the ways a buffer will be used. Some combinations aren't valid.
enum class RhiBufferUsage
{
	TransferSrc = 1 << 0,
	TransferDest = 1 << 1,
	VertexBuffer = 1 << 2,
	IndexBuffer = 1 << 3,
	UniformBuffer = 1 << 4,
	IndirectBuffer = 1 << 5,
	// TODO: ray tracing stuff
};

/// Represents any memory buffer controller by the graphics API
class IRhiBuffer
{
  public:
	virtual ~IRhiBuffer() = default;

	/// Get the size of this buffer
	virtual ssize GetSize() = 0;

    /// Get the allowed usage of this buffer
    virtual RhiBufferUsage GetUsage() = 0;

	/// Map the buffer into host address space
	virtual uptr Map(u64 size, u64 baseOffset = 0) = 0;

	/// Map the buffer as a specific type
	template <typename T> T* Map(u64 size, u64 baseOffset = 0)
	{
		return reinterpret_cast<T*>(Map(size, baseOffset));
	}

	/// Unmap the buffer
	virtual void Unmap(uptr hostAddr, u64 size) = 0;
};
