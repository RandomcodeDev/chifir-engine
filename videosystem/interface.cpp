/// \file DLL interface for video system
/// \copyright 2025 Randomcode Developers

#include "isystem.h"
#include "video_null.h"
#include "video_sdl.h"
#include "videosystem/ivideosystem.h"

#if defined CH_WIN32 && !defined CH_XBOX360
#include "video_win32.h"
#include "video_winrt.h"
#elif defined CH_SWITCH
#include "videosystem/video_switch.h"
#elif defined CH_UNIX
#include "video_sdl.h"
#endif

#ifdef CH_STATIC
ISystem* CreateVideoSystem()
#else
extern "C" DLLEXPORT ISystem* CreateInterface()
#endif
{
#if defined CH_WIN32 && !defined CH_XBOX360
	if (Plat_IsUwpApp())
	{
		return new CWinRtVideoSystem();
	}
	else
	{
		return new CWin32VideoSystem();
	}
#elif defined CH_SWITCH
	return new CSwitchVideoSystem();
#elif defined CH_UNIX
	return new CSdlVideoSystem();
#else
	return new CNullVideoSystem();
#endif
}
