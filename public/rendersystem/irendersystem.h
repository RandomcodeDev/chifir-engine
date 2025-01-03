/// Render system interface

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "isystem.h"

class IVideoSystem;

class IRenderSystem : public ISystem
{
  public:
	virtual ~IRenderSystem() DEFAULT;

	virtual bool Initialize(IVideoSystem* videoSystem) = 0;
	virtual void Shutdown() = 0;

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
	virtual bool Initialize() { return false; };
};
