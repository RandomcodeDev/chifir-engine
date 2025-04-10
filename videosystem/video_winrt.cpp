/// \file WinRT video system
/// \copyright Randomcode Developers

#include "video_winrt.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/log.h"

bool CWinRtVideoSystem::Initialize()
{
	m_window = Plat_GetUwpWindow();
	return true;
}

bool CWinRtVideoSystem::Update()
{
	m_resized = false;

	

	return !m_closed;
}

void CWinRtVideoSystem::Shutdown()
{
	Log_Info("WinRT video system shut down");
}

void CWinRtVideoSystem::SetTitle(cstr newTitle)
{
	m_title = Base_StrClone(newTitle);
	if (m_window)
	{
		
	}
}
