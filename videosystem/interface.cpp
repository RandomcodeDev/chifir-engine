/// \file DLL interface for video system
/// \copyright Randomcode Developers

#include "isystem.h"
#include "video_null.h"
#include "videosystem/ivideosystem.h"

#if defined CH_WIN32 && !defined CH_XBOX360
#include "video_win32.h"
#include "video_winrt.h"
#elif defined CH_SWITCH
#include "videosystem/video_switch.h"
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
#else
	return new CNullVideoSystem();
#endif
}
