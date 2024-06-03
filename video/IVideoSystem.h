/// @file IVideoSystem.h
/// @brief This file defines the video system interface
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"
#include "framework/ISystem.h"

class IVideoSystem : public ISystem
{
  public:
	virtual ~IVideoSystem() = default;

	/// @brief Create a place for rendering to go
	virtual bool Initialize() = 0;

#ifdef KR_RENDER_VULKAN
	/// @brief Create a Vulkan surface
	virtual u64 CreateVulkanSurface(u64 instance, void* allocationCallbacks) = 0;
#endif

	/// @brief Update video
	/// @return Whether the video output was closed
	virtual bool Update() = 0;

	/// @brief Shut down and clean up
	virtual void Shutdown() = 0;

	/// @brief Get the title of the video output
	virtual const std::string& GetTitle() const = 0;

	/// @brief Set the title of the video output
	virtual void SetTitle(const std::string& title) = 0;

	/// @brief Get the width in pixels of the video output
	virtual u32 GetWidth() const = 0;

	/// @brief Get the height in pixels of the video output
	virtual u32 GetHeight() const = 0;

	/// @brief Get the X position of the video output (mainly for cursor position)
	virtual u32 GetX() const = 0;

	/// @brief Get the Y position of the video output
	virtual u32 GetY() const = 0;

	/// @brief Get whether the window has been resized since the last update
	virtual bool Resized() const = 0;

	/// @brief Get whether the window is focused
	virtual bool Focused() const = 0;

	/// @brief Get the current DPI
	virtual float GetDpi() const = 0;

	/// @brief Get the platform-specific handle (HWND, etc) for the video output
	virtual u64 GetHandle() const = 0;

	static constexpr u32 VERSION = 1;
};
