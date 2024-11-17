// DLL interface for video system

#include "isystem.h"
#ifdef CH_XBOX360
#include "video_null.h"
#elif defined CH_WIN32
#include "video_win32.h"
#endif

const cstr IVideoSystem::NAME = "VideoSystem";

#ifdef CH_STATIC
ISystem* CreateVideoSystem()
#else
extern "C" DLLEXPORT ISystem* CreateInterface()
#endif
{
#ifdef CH_XBOX360
	return new CNullVideoSystem();
#elif defined CH_WIN32
	return new CWindowsVideoSystem();
#else
	return nullptr;
#endif
}
