// This file defines the interface for abstracting a graphics API's idea of a context or instance

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "base/vector.h"

struct RhiDeviceInfo_t;
class IRhiDevice;
class IRhiSwapChain;

// Represents a DXGI factory, a VkInstance, or whatever ungodly thing legacy APIs use (GL context, etc)
class IRhiInstance
{
	virtual ~IRhiInstance() DEFAULT;

	// Destroys the instance, all objects created off of it must be destroyed before this
	virtual void Destroy() = 0;

	// Get information about the graphics devices present
	virtual CVector<RhiDeviceInfo_t> GetDeviceInfo() = 0;

	// Create a graphics device
	virtual IRhiDevice* CreateDevice() = 0;

	// Create a swap chain
	virtual IRhiSwapChain* CreateSwapChain() = 0;
};
