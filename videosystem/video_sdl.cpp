/// \file SDL video system
/// \copyright 2025 Randomcode Developers

#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

#include "base/base.h"
#include "base/basicstr.h"
#include "base/log.h"
#include "video_sdl.h"
#include "base/platform.h"
#include "vulkan/vulkan_core.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

bool CSdlVideoSystem::Initialize()
{
	Log_Debug("Initializing SDL video system");

	m_title = Base_StrClone(GAME_DISPLAY_NAME);
	if (!m_title)
	{
		return false;
	}

	m_width = 1280;
	m_height = 720;
	m_x = 0;
	m_y = 0;

	Log_Info("Creating %ux%u window titled %s", m_width, m_height, m_title);
	m_handle = SDL_CreateWindow(m_title, m_width, m_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);
	if (!m_handle)
	{
		Log_Error("Failed to create window: %s", SDL_GetError());
		return false;
	}

	SDL_GetWindowPosition(m_handle, &m_x, &m_y);

	m_resized = false;
	m_focused = true;
	m_closed = false;
	m_dpi = 1.0;

	return true;
}

bool CSdlVideoSystem::Update()
{
	m_resized = false;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_WINDOW_RESIZED:
			if (m_width != event.window.data1 || m_height != event.window.data2)
			{
				Log_Trace("Window resized from %ux%u to %ux%u", m_width, m_height, event.window.data1, event.window.data2);
				m_width = event.window.data1;
				m_height = event.window.data2;
				m_resized = true;
			}
			break;
		case SDL_EVENT_WINDOW_MOVED:
			m_x = event.window.data1;
			m_y = event.window.data2;
			break;
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			Log_Debug("Window focused");
			m_focused = true;
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			Log_Debug("Window lost focus");
			m_focused = false;
			break;
		case SDL_EVENT_QUIT:
			Log_Debug("Quit requested");
			m_closed = true;
			break;
		}
	}

	return !m_closed && !Plat_QuitSignalled();
}

void CSdlVideoSystem::Shutdown()
{
	Log_Debug("Shutting down SDL video system");

	if (m_handle)
	{
		SDL_DestroyWindow(m_handle);
	}

	if (m_title)
	{
		Base_Free(m_title);
	}
}

u64 CSdlVideoSystem::CreateVulkanSurface(u64 instance, const void* allocCallbacks)
{
	Log_Debug("Creating SDL Vulkan surface");

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	if (!SDL_Vulkan_CreateSurface(
			m_handle, reinterpret_cast<VkInstance>(instance), reinterpret_cast<const VkAllocationCallbacks*>(allocCallbacks),
			&surface))
	{
		Log_Error("Failed to create SDL Vulkan surface: %s", SDL_GetError());
		return 0;
	}

	return reinterpret_cast<u64>(surface);
}

void CSdlVideoSystem::SetTitle(cstr newTitle)
{
	m_title = Base_StrClone(newTitle);
}
