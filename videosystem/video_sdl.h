/// \file SDL video system
/// \copyright Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "videosystem/isdlvideosystem.h"
#include "videosystem/ivideosystem.h"

struct SDL_Window;

class CSdlVideoSystem: public IVideoSystem, public ISdlVideoSystem
{
  public:
	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();

    virtual u64 CreateVulkanSurface(u64 instance, const void* allocCallbacks);

	virtual cstr GetName() const
	{
		return "SDL Video";
	}

	virtual u32 GetVersion() const
	{
		return VERSION;
	}

	virtual cstr GetTitle() const
	{
		return m_title;
	}

	virtual void SetTitle(cstr newTitle);

	virtual u32 GetWidth() const
	{
		return m_width;
	}

	virtual u32 GetHeight() const
	{
		return m_height;
	}

	virtual u32 GetX() const
	{
		return static_cast<u32>(m_x);
	}

	virtual u32 GetY() const
	{
		return static_cast<u32>(m_y);
	}

	virtual bool Resized() const
	{
		return m_resized;
	}

	virtual bool Focused() const
	{
		return m_focused;
	}

	virtual float GetDpi() const
	{
		return m_dpi;
	}

	virtual u64 GetHandle() const
	{
		return reinterpret_cast<u64>(m_handle);
	}

  private:
	SDL_Window* m_handle;
	dstr m_title;
	u32 m_width;
	u32 m_height;
	s32 m_x;
	s32 m_y;
	bool m_resized;
	bool m_focused;
	bool m_closed;
	f32 m_dpi;
};
