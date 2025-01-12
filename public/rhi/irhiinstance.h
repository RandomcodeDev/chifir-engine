/// This file defines the interface for abstracting a graphics API's idea of a context or instance

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "base/vector.h"

#include "irhidevice.h"
#include "rhi.h"

class IRhiSwapChain;

/// Represents a DXGI factory, a VkInstance, or whatever ungodly thing legacy APIs use (GL context, LPDIRECT3D9, etc)
class RHIAPI IRhiInstance
{
  public:
	virtual ~IRhiInstance() DEFAULT;

	/// Initializes the instance, must be called before the instance is used
	virtual bool Initialize() = 0;

	/// Destroys the instance, all objects created off of it must be destroyed before this
	virtual void Destroy() = 0;

	/// Get information about the graphics devices present
	virtual void GetDeviceInfo(CVector<RhiDeviceInfo_t>& info) = 0;

	/// Create a graphics device
	virtual IRhiDevice* CreateDevice(s32 index = 0) = 0;

	/// Create a swap chain
	virtual IRhiSwapChain* CreateSwapChain() = 0;
};

/// Create an IRhiInstance
extern RHIAPI IRhiInstance* Rhi_CreateInstance(RhiBackendType_t type = RhiBackendType_t::Vulkan);
