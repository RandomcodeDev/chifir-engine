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

	u8* block = Base_Alloc<u8>(1 * 1024 * 1024 * 1024);
	Base_MemSet(block, 0xFF, 1024);

	void* block2 = Base_Alloc(1024);
	Base_Free(block);
	void* block3 = Base_Alloc(1024);

	Base_Free(block3);
	Base_Free(block2);

	Plat_Shutdown();
	Base_Shutdown();

	return 0;
}
