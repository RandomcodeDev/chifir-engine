/// Video system interface

#pragma once

#include "base/compiler.h"
#include "base/types.h"

#include "isystem.h"

/// The video system is responsible for creating a window or setting the mode of a console's screen, and allowing graphics APIs to
/// interface with it
class IVideoSystem: public ISystem
{
  public:
	virtual ~IVideoSystem() DEFAULT;

	/// Set up an output for rendering
	virtual bool Initialize() = 0;

	/// Process events, returns true if the video output hasn't been closed
	virtual bool Update() = 0;

	/// Shut down the video system
	virtual void Shutdown() = 0;

	/// Get the title
	virtual cstr GetTitle() const = 0;

	/// Set the title
	virtual void SetTitle(cstr newTitle) = 0;

	//// Get the width in pixels of the video output
	virtual u32 GetWidth() const = 0;

	//// Get the height in pixels of the video output
	virtual u32 GetHeight() const = 0;

	//// Get the X position of the video output (mainly for cursor position)
	virtual u32 GetX() const = 0;

	//// Get the Y position of the video output
	virtual u32 GetY() const = 0;

	//// Get whether the window has been resized since the last update
	virtual bool Resized() const = 0;

	//// Get whether the window is focused
	virtual bool Focused() const = 0;

	//// Get the current DPI
	virtual float GetDpi() const = 0;

	//// Get the platform-specific handle (HWND, etc) for the video output
	virtual u64 GetHandle() const = 0;

	static const u32 VERSION = 1;
};

#ifdef CH_STATIC
extern ISystem* CreateVideoSystem();
#endif
