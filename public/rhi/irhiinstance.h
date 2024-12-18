// This file defines the interface for abstracting a graphics API's idea of a context or instance

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "base/vector.h"
#include "irhidevice.h"

class IRhiSwapChain;

// Represents a DXGI factory, a VkInstance, or whatever ungodly thing legacy APIs use (GL context, LPDIRECT3D9, etc)
class IRhiInstance
{
	virtual ~IRhiInstance() DEFAULT;

	// Destroys the instance, all objects created off of it must be destroyed before this
	virtual void Destroy() = 0;

	// Get information about the graphics devices present
	virtual CVector<RhiDeviceInfo_t> GetDeviceInfo() = 0;

	// Create a graphics device
	virtual IRhiDevice* CreateDevice(s32 index = 0) = 0;

	// Create a swap chain
	virtual IRhiSwapChain* CreateSwapChain() = 0;
};
