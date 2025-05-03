/// \file SDL video system interface
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "videosystem/ivideosystem.h"

class ISdlVideoSystem: public IVideoSystem
{
  public:
	virtual ~ISdlVideoSystem() = default;

	virtual bool Initialize() = 0;
	virtual bool Update() = 0;
	virtual void Shutdown() = 0;
	virtual u64 CreateVulkanSurface(u64 instance, const void* allocCallbacks) = 0;
	virtual cstr GetName() const = 0;
	virtual u32 GetVersion() const = 0;
	virtual cstr GetTitle() const = 0;
	virtual void SetTitle(cstr newTitle) = 0;
	virtual u32 GetWidth() const = 0;
	virtual u32 GetHeight() const = 0;
	virtual u32 GetX() const = 0;
	virtual u32 GetY() const = 0;
	virtual bool Resized() const = 0;
	virtual bool Focused() const = 0;
	virtual float GetDpi() const = 0;
	virtual u64 GetHandle() const = 0;
};
