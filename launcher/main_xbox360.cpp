/// \file Xbox 360 launcher
/// \copyright Randomcode Developers

#include "base/base.h"

#include "launcher.h"

int main()
{
	extern int LauncherMain();

	Base_Init();
	int result = LauncherMain();
	Base_Shutdown();

	return result;
}
