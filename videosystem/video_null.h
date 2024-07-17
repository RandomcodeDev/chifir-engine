// Dummy video system

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "videosystem/ivideosystem.h"

class CNullVideoSystem : public IVideoSystem
{
  public:
	bool Initialize();
	bool Update();
	void Shutdown();

	u32 GetVersion() const
	{
		return VERSION;
	}

	cstr GetTitle() const
	{
		return m_title;
	}

	void SetTitle(cstr newTitle);

	u32 GetWidth() const
	{
		return m_width;
	}

	u32 GetHeight() const
	{
		return m_height;
	}

	u32 GetX() const
	{
		return m_x;
	}

	u32 GetY() const
	{
		return m_y;
	}

	bool Resized() const
	{
		return m_resized;
	}

	bool Focused() const
	{
		return m_focused;
	}

	float GetDpi() const
	{
		return m_dpi;
	}

	u64 GetHandle() const
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
