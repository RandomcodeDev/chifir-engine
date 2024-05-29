#include "framework/Util.h"

#include "video/IVideoSystem.h"
#ifdef KR_PLATFORM_WINDOWS
#include "video/impl/windows/CWindowsVideoSystem.h"
#endif

extern "C" KR_EXPORT ISystem* CreateInterface()
{
#ifdef KR_PLATFORM_WINDOWS
	return new CWindowsVideoSystem();
#else
	KR_QUIT("No video system implementation for " KR_PLATFORM "!");
#endif
}

extern "C" KR_EXPORT u32 GetSystemVersion()
{
	return IVideoSystem::VERSION;
}
