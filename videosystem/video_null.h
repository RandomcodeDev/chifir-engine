// Dummy video system

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "videosystem/ivideosystem.h"

class CNullVideoSystem: public IVideoSystem
{
  public:
	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();

	virtual cstr GetName() const
	{
		return "Null Video";
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
		return m_x;
	}

	virtual u32 GetY() const
	{
		return m_y;
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
		return 0;
	}

  private:
	dstr m_title;
	u32 m_width;
	u32 m_height;
	u32 m_x;
	u32 m_y;
	bool m_resized;
	bool m_focused;
	bool m_closed;
	f32 m_dpi;
};
