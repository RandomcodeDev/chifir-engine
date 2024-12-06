#include "base/base.h"
#include "base/basicstr.h"
#include "base/log.h"
#include "video_null.h"

bool CNullVideoSystem::Initialize()
{
	Log_Debug("Initializing null video system");

	m_title = Base_StrClone("Чифир Engine");
	if (!m_title)
	{
		return false;
	}

	m_width = 1280;
	m_height = 720;
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
