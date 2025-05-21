/// \file DLL interface for video system
/// \copyright 2025 Randomcode Developers

#include "isystem.h"
#include "video_null.h"
#include "video_sdl.h"
#include "videosystem/ivideosystem.h"

#ifdef CH_WIN32
#include "video_win32.h"
#include "video_winrt.h"
#elif defined CH_XBOX
#include "video_xbox.h"
#elif defined CH_NX
#include "videosystem/video_nx.h"
#elif defined CH_UNIX
#include "video_sdl.h"
#endif

#ifdef CH_STATIC
ISystem* CreateVideoSystem()
#else
extern "C" DLLEXPORT ISystem* CreateInterface()
#endif
{
#if defined CH_WIN32
#ifdef CH_UWP
	if (Plat_IsUwpApp())
	{
		return new CWinRtVideoSystem();
	}
	else
#endif
	{
		return new CWin32VideoSystem();
	}
#elif defined CH_XBOX
    return new CXboxVideoSystem();
#elif defined CH_NX
	return new CNxVideoSystem();
#elif defined CH_UNIX
	return new CSdlVideoSystem();
#else
	return new CNullVideoSystem();
#endif
}
