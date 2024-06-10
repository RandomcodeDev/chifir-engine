/// @file INVRHIDeviceManager.h
/// @brief This file defines the interface for API-specific functionality required by NVRHI.
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"

#include "nvrhi/nvrhi.h"

class IVideoSystem;

class INVRHIDeviceManager
{
  public:
	/// @brief Destroy any objects
	virtual ~INVRHIDeviceManager() = default;

	/// @brief Create a device
	virtual nvrhi::DeviceHandle CreateDevice() = 0;

	/// @brief Create the swap chain
	virtual bool CreateSwapChain(IVideoSystem* videoSystem) = 0;

	/// @brief Destroy the swap chain
	virtual void DestroySwapChain() = 0;

	/// @brief Begin a frame
	virtual void BeginFrame(IVideoSystem* videoSystem) = 0;

	/// @brief End the frame and present it
	virtual void Present() = 0;

	/// @brief Set the GPU index
	virtual bool SetGpu(size_t index = 0) = 0;

	/// @brief Get the name of the GPU
	virtual const std::string& GetGpuName() = 0;

	/// @brief Get the index of the GPU
	virtual size_t GetGpuIndex() = 0;
};
