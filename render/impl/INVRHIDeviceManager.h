/// @file INVRHIDeviceManager.h
/// @brief This file defines the interface for API-specific functionality required by NVRHI.
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"

#include "nvrhi/nvrhi.h"

class INVRHIDeviceManager
{
  public:
	virtual ~INVRHIDeviceManager() = default;

	/// @brief Create a device
	virtual nvrhi::DeviceHandle CreateDevice() = 0;
};
