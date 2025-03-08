/// Render system interface

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "isystem.h"

class IVideoSystem;

class IRenderSystem: public ISystem
{
  public:
	virtual ~IRenderSystem() DEFAULT;

	/// Initialize the render system
	virtual bool Initialize(IVideoSystem* videoSystem) = 0;
	/// Shut down the render system
	virtual void Shutdown() = 0;

	/// Change the current device, requires reinitialization if not successful
	virtual bool ChangeDevice(u32 index) = 0;

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual cstr GetName() const
	{
		return "Render";
	}

	virtual u32 GetVersion() const
	{
		return VERSION;
	}

	static const u32 VERSION = 1;

  private:
	virtual bool Initialize()
	{
		return false;
	};
};

#ifdef CH_STATIC
extern ISystem* CreateRenderSystem();
#endif
