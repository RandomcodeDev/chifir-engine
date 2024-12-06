// DLL interface for video system

#include "isystem.h"
#include "video_null.h"
#include "videosystem/ivideosystem.h"

#if defined CH_WIN32 && !defined CH_XBOX360
#include "video_win32.h"
#elif defined CH_SWITCH
#include "videosystem/video_switch.h"
#endif

const cstr IVideoSystem::NAME = "VideoSystem";

#ifdef CH_STATIC
ISystem* CreateVideoSystem()
#else
extern "C" DLLEXPORT ISystem* CreateInterface()
#endif
{
#if defined CH_WIN32 && !defined CH_XBOX360
	return new CWindowsVideoSystem();
#elif defined CH_SWITCH
	return new CSwitchVideoSystem();
#else
	return new CNullVideoSystem();
#endif
}
