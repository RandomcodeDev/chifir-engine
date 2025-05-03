/// \file RHI semaphore interface
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "rhi.h"

// ID3D12Fence/VkSemaphore
class IRhiSemaphore
{
  public:
	virtual ~IRhiSemaphore() DEFAULT;

	// Destroy the semaphore
	virtual void Destroy() = 0;

	// Get the semaphore's value
	virtual u64 GetValue() = 0;

	// Set the semaphore's value
	virtual void SetValue(u64 value) = 0;

	// Incrememnt the semaphore
	void Increment()
	{
		SetValue(GetValue() + 1);
	}

	// Set the target value before the semaphore is signalled
    virtual void SetTarget(u64 value) = 0;
};
