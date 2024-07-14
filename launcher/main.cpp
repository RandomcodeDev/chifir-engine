#include "base/loader.h"
#include "base/platform.h"
// #include "base/vector.h"
#include "base/types.h"
#include "isystem.h"
#include "launcher.h"
#include "video/ivideosystem.h"

s32 LauncherMain()
{
	Base_Init();
	Plat_Init();

	ILibrary* videoSystemDll = Base_LoadLibrary("VideoSystem");
	CreateInterface_t createInterface = videoSystemDll->GetSymbol<CreateInterface_t>("CreateInterface");
	IVideoSystem* videoSystem = reinterpret_cast<IVideoSystem*>(createInterface());
	videoSystem->Initialize();
	while (videoSystem->Update())
	{

	}
	videoSystem->Shutdown();
	delete videoSystem;
	delete videoSystemDll;

	Plat_Shutdown();
	Base_Shutdown();

	return 0;
}
