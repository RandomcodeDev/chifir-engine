/// \file WinRT video system
/// \copyright Randomcode Developers

#include "video_winrt.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/log.h"

bool CWinRtVideoSystem::Initialize()
{
	UwpVideoInfo info = {};
	
	UwpVideoCallbacks callbacks = {
		this,

		OnSizeChanged,
		OnClosed
	};

	Plat_BindUwpVideo(info, callbacks);

	m_window = info.window;
	m_dispatcher = info.dispatcher;

	return true;
}

bool CWinRtVideoSystem::Update()
{
	m_resized = false;

	m_dispatcher->ProcessEvents(winrt_min::CoreProcessEventsOption::ProcessAllIfPresent);

	return !m_closed;
}

void CWinRtVideoSystem::Shutdown()
{
	Log_Info("WinRT video system shut down");
}

void CWinRtVideoSystem::SetTitle(cstr newTitle)
{
	UNUSED(newTitle);
	Log_Warning("SetTitle is unsupported on UWP");
}

bool CWinRtVideoSystem::OnSizeChanged(f32 width, f32 height, void* user)
{
	auto videoSystem = reinterpret_cast<CWinRtVideoSystem*>(user);
	
	if (width != videoSystem->m_width || height != videoSystem->m_height)
	{
		Log_Trace("Window resized from %ux%u to %.0fx%.0f", videoSystem->m_width, videoSystem->m_height, width, height);
		videoSystem->m_width = static_cast<u32>(width);
		videoSystem->m_height = static_cast<u32>(height);
	}

	return true;
}

bool CWinRtVideoSystem::OnClosed(void* user)
{
	auto videoSystem = reinterpret_cast<CWinRtVideoSystem*>(user);

	Log_Debug("Window closed");
	videoSystem->m_closed = true;

	return true;
}
