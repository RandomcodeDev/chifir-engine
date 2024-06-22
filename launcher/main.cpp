#include "base/types.h"
#include "launcher.h"
#include "platform/loader.h"
#include "platform/platform.h"

// TODO: add command line argument
s32 LauncherMain()
{
	Plat_Init();

	Plat_Shutdown();

	return 0;
}
