/// This file defines the interface for abstracting a graphics API's idea of a context or instance

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "base/vector.h"

#include "irhidevice.h"
#include "rhi.h"

class IVideoSystem;

/// Represents a DXGI factory, a VkInstance, or whatever ungodly thing legacy APIs use (GL context, LPDIRECT3D9, etc)
class IRhiInstance
{
  public:
	virtual ~IRhiInstance() DEFAULT;

	/// Initializes the instance, must be called before the instance is used
	virtual bool Initialize(IVideoSystem* videoSystem) = 0;

	/// Destroys the instance, all objects created off of it must be destroyed before this
	virtual void Destroy() = 0;

	/// Get information about the graphics devices present
	virtual void GetDeviceInfo(CVector<RhiDeviceInfo_t>& info) = 0;

	/// Create a graphics device, the info must be from a successful call to GetDeviceInfo
	virtual IRhiDevice* CreateDevice(const RhiDeviceInfo_t& info) = 0;
};

/// Create an IRhiInstance
extern "C" RHIAPI IRhiInstance* Rhi_CreateInstance(RhiBackendType_t type = RhiBackendType_t::Vulkan);

/// Exported by RHI backends as "CreateInstance"
typedef IRhiInstance* (*CreateRhiInstance_t)();
