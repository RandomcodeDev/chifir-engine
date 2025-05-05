/// \file Stubbed video system
/// \copyright 2025 Randomcode Developers

#include "video_null.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/log.h"

bool CNullVideoSystem::Initialize()
{
	Log_Debug("Initializing null video system");

	m_title = Base_StrClone(GAME_NAME);
	if (!m_title)
	{
		return false;
	}

#ifdef CH_XBOX
	m_width = 640; // TODO: does it support 16:9?
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

bool CNullVideoSystem::Update()
{
	return !m_closed;
}

void CNullVideoSystem::Shutdown()
{
	Base_Free(m_title);
	Log_Debug("Shutting down null video system");
}

void CNullVideoSystem::SetTitle(cstr newTitle)
{
	m_title = Base_StrClone(newTitle);
}
