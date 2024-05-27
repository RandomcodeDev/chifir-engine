#include "framework/Framework.h"
#include "framework/CCommandLine.h"
#include "framework/IApplication.h"
#include "framework/IPlatform.h"
#include "framework/ISharedLibrary.h"
#include "framework/Log.h"
#include "framework/Util.h"

#if defined KR_PLATFORM_WINDOWS && !defined KR_DEBUG
extern "C" int WinMain(HINSTANCE instance, HINSTANCE prevInstance, char* cmdline, int show)
{
	CCommandLine cmdLine(GetCommandLineA());
#else
extern "C" int main(int argc, char* argv[])
{
	CCommandLine cmdLine(argc, argv);
#endif

	GetPlatform()->Initialize();

	KR_LOG_INFO("Running on {}", GetPlatform()->DescribeOs());

	ISharedLibrary* appLib = GetPlatform()->LoadLibrary("Engine");
	if (!appLib)
	{
		KR_QUIT("Failed to load engine!");
	}

	auto createInterface = appLib->GetFunction<IApplication*>("CreateInterface");
	if (!createInterface)
	{
		KR_QUIT("Invalid engine library!");
	}

	IApplication* app = createInterface();
	if (!app)
	{
		KR_QUIT("Failed to create application interface!");
	}

	KR_LOG_INFO("Loaded app {} ({}) by {}", app->GetName(), app->GetIdName(), app->GetOrganization());

	delete app;
	delete appLib;

	GetPlatform()->Shutdown();

	return 0;
}
