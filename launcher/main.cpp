#include "base/base.h"
#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"
#include "launcher.h"

// TODO: add command line argument
s32 LauncherMain()
{
	Base_Init();
	Plat_Init();

	char a[1029];
	char b[1029];
	Base_MemSet(a + 5, 5, 1024);
	Base_MemSet(b + 5, 10, 1024);
	Base_MemCpy(b + 5, a + 5, 1024);

	Plat_Shutdown();
	Base_Shutdown();

	return 0;
}
