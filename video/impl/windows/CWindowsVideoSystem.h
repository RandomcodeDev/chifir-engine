/// @file CWindowsVideoSystem.h
/// @brief This file defines the Windows implementation of the video system
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"
#include "video/IVideoSystem.h"

class CWindowsVideoSystem : public IVideoSystem
{
  public:
	CWindowsVideoSystem() = default;
	~CWindowsVideoSystem() = default;
	bool Initialize();
	bool Update();
	void Shutdown();

	const std::string& GetTitle() const
	{
		return m_title;
	}

	void SetTitle(const std::string& title);

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
		return (u64)m_window;
	}

  private:
	static constexpr const char* WINDOW_CLASS = "КраснийWindow";
	static constexpr u32 WINDOW_STYLE = WS_OVERLAPPEDWINDOW;

	HINSTANCE m_hinstance;
	HWND m_window;
	std::string m_title;
	u32 m_width;
	u32 m_height;
	u32 m_extraWidth;
	u32 m_extraHeight;
	u32 m_x;
	u32 m_y;
	bool m_resized;
	bool m_focused;
	bool m_closed;
	float m_dpi;

	bool RegisterWindowClass();
	bool InitializeMainWindow();
	void UpdateSize();
	void UpdatePosition();
	void UpdateDpi();

	static LRESULT WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
};
