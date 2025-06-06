/// \file WinRT video system
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"
#include "videosystem/ivideosystem.h"

class CWinRtVideoSystem: public IVideoSystem
{
  public:
	CWinRtVideoSystem()
		: m_window(nullptr), m_title(nullptr), m_width(0), m_height(0), m_x(0), m_y(0), m_resized(false), m_focused(false),
		  m_closed(false), m_dpi(1.0)
	{
	}
	~CWinRtVideoSystem() = default;

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();

	virtual cstr GetName() const
	{
		return "WinRT Video";
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
		return reinterpret_cast<uptr>(m_window);
	}

  private:
	winrt_min::ICoreWindow* m_window;
	winrt_min::ICoreDispatcher* m_dispatcher;
	dstr m_title;
	u32 m_width;
	u32 m_height;
	u32 m_x;
	u32 m_y;
	bool m_resized;
	bool m_focused;
	bool m_closed;
	f32 m_dpi;

	static bool OnSizeChanged(f32 width, f32 height, void* user);
	static bool OnClosed(void* user);
};

#define WINDOW_CLASS "ChifirWindow"
