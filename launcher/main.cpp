#include "base/base.h"
#include "base/basicstr.h"
#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"
#include "launcher.h"
s32 LauncherMain()
{
	Base_Init();
	Plat_Init();

	Plat_Shutdown();
	Base_Shutdown();

	return 0;
}
