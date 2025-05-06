/// \file Xbox launcher
/// \copyright 2025 Randomcode Developers

#include "base/base.h"

#include "launcher.h"

#ifdef CH_XENON
#pragma comment(linker, "/SUBSYSTEM:XBOX,2.00")
#endif

int main()
{
	extern int LauncherMain();

	Base_Init();
	int result = LauncherMain();
	Base_Shutdown();

	return result;
}
