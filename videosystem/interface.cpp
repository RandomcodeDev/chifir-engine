// DLL interface for video system

#include "isystem.h"
#ifdef CH_WIN32
#include "video_win32.h"
#endif

extern "C" DLLEXPORT ISystem* CreateInterface()
{
#ifdef CH_XBOX360
	return nullptr;
#elif defined CH_WIN32
	return new CWindowsVideoSystem();
#else
	return nullptr;
#endif
}
