// DLL interface for video system

#include "isystem.h"
#ifdef CH_XBOX360
#include "video_null.h"
#elif defined CH_WIN32
#include "video_win32.h"
#endif

extern "C"
#ifndef CH_STATIC
DLLEXPORT
#endif
ISystem* CreateInterface()
{
#ifdef CH_XBOX360
	return new CNullVideoSystem();
#elif defined CH_WIN32
	return new CWindowsVideoSystem();
#else
	return nullptr;
#endif
}
