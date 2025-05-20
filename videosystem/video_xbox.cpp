/// \file Xbox video system
/// \copyright 2025 Randomcode Developers

#include "base/base.h"
#include "base/basicstr.h"
#include "base/log.h"

#include "video_xbox.h"

bool CXboxVideoSystem::Initialize()
{
	Log_Debug("Initializing Xbox video system");

	m_title = Base_StrClone(GAME_DISPLAY_NAME);
	if (!m_title)
	{
		return false;
	}

    // TODO: get video settings
#ifdef CH_XBOX
	m_width = 640;
	m_height = 480;
#else
	m_width = 1280;
	m_height = 720;
#endif
	m_x = 0;
	m_y = 0;
	m_resized = false;
	m_focused = true;
	m_closed = false;
	m_dpi = 1.0;

	return true;
}

bool CXboxVideoSystem::Update()
{
	return !m_closed;
}

void CXboxVideoSystem::Shutdown()
{
	Base_Free(m_title);
	Log_Debug("Shutting down Xbox video system");
}

void CXboxVideoSystem::SetTitle(cstr newTitle)
{
	m_title = Base_StrClone(newTitle);
}
