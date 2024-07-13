// DLL interface for video system

#include "isystem.h"
#ifdef CH_WIN32
//#include "video_win32.h"
#endif

extern "C" ISystem* CreateInterface()
{
#ifdef CH_XBOX360
#elif defined CH_WIN32
	//return new CWindowsVideoSystem();
#endif

	return nullptr;
}
