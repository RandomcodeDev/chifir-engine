/// \file RHI memory buffers
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "irhimemory.h"

/// Represents the ways a buffer will be used. Some combinations aren't valid.
enum class RhiBufferUsage
{
	TransferSrc,
	TransferDest,
	VertexBuffer,
	IndexBuffer,
	UniformBuffer,
	IndirectBuffer,
	// TODO: ray tracing stuff
};

/// Represents any memory buffer controller by the graphics API
class IRhiBuffer
{
  public:
	virtual ~IRhiBuffer() = default;

	/// Get the memory that backs this buffer
	virtual IRhiMemory* GetMemory() = 0;

	/// Get the size of this buffer
	virtual ssize GetSize() = 0;
};

/// A buffer that the host can see
class IRhiHostBuffer: public IRhiBuffer
{
  public:
	virtual ~IRhiHostBuffer() = default;

	virtual IRhiMemory* GetMemory() = 0;
	virtual ssize GetSize() = 0;

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
