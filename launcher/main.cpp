#include "base/loader.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"
#include "iapplication.h"
#include "isystem.h"
#include "launcher.h"
#include "utility/utility.h"
#include "videosystem/ivideosystem.h"

#ifdef CH_STATIC
extern IApplication* CreateEngine();
#else
static IApplication* GetApplication(ILibrary* library, CVector<SystemDependency_t>& dependencies)
{
	CreateApplicationInterface_t CreateInterface = library->GetSymbol<CreateApplicationInterface_t>("CreateInterface");
	if (!CreateInterface)
	{
		Log_Error("Failed to get CreateInterface in library %s", library->GetName());
		return nullptr;
	}

	IApplication* app = CreateInterface();
	if (!app)
	{
		Log_Error("Failed to create application from library %s", library->GetName());
		return nullptr;
	}

	app->GetRequiredSystems(dependencies);

	return app;
}

static ISystem* GetSystem(ILibrary* library, u32 minVersion, bool exactRequired)
{
	CreateSystemInterface_t CreateInterface = library->GetSymbol<CreateSystemInterface_t>("CreateInterface");
	if (!CreateInterface)
	{
		Log_Error("Failed to get CreateInterface in library %s", library->GetName());
		return nullptr;
	}

	ISystem* system = CreateInterface();
	if (!system)
	{
		Log_Error("Failed to create system from library %s", library->GetName());
		return nullptr;
	}

	// TODO: simplify
	if (exactRequired && system->GetVersion() != minVersion)
	{
		Log_Error("System version is %u, version %u is required", system->GetVersion(), minVersion);
		delete system;
		return nullptr;
	}
	else if (system->GetVersion() < minVersion)
	{
		Log_Error("System version is %u, version %u or higher is required", system->GetVersion(), minVersion);
		delete system;
		return nullptr;
	}

	return system;
}
#endif

extern "C" LAUNCHERAPI s32 LauncherMain()
{
#ifdef CH_XBOX360
	__security_init_cookie();
#endif

	Base_Init();
	Plat_Init();

#ifdef CH_WIN32
	Log_AddWriter(new CDbgPrintLogWriter());
#ifndef CH_XBOX360
	Log_AddWriter(new CWin32ConsoleLogWriter());
#endif
#endif

	cstr appName = "Engine"; // TODO: make this based on a command line arg

#ifdef CH_STATIC
	CVector<ISystem*> systems;
	systems.Add(CreateVideoSystem());

	IApplication* app = CreateEngine();
#else
	Log_Info("Loading application %s", appName);
	ILibrary* appLib = Base_LoadLibrary(appName);
	if (!appLib)
	{
		Base_Quit("Failed to load application %s!", appName);
	}

	Log_Info("Initializing application %s", appName);
	CVector<SystemDependency_t> appDependencies;
	IApplication* app = GetApplication(appLib, appDependencies);
	if (!app)
	{
		Base_Quit("Failed to initialize application %s!", appName);
	}

	Log_Info("Loading systems for application %s", appName);
	CVector<ILibrary*> libs;
	CVector<ISystem*> systems;
	for (ssize i = 0; i < appDependencies.Size(); i++)
	{
		Log_Info(
			"Loading %s, %sversion %u", appDependencies[i].name, appDependencies[i].requireExactVersion ? "" : "minimum ",
			appDependencies[i].minimumVersion);
		ILibrary* lib = Base_LoadLibrary(appDependencies[i].name);
		if (!lib)
		{
			Base_Quit("Failed to load system %s!", appDependencies[i].name);
		}
		libs.Add(lib);

		Log_Info("Getting interface for %s", appDependencies[i].name);
		ISystem* system = GetSystem(lib, appDependencies[i].minimumVersion, appDependencies[i].requireExactVersion);
		if (!system)
		{
			Base_Quit("Failed to get interface for %s, or it's the wrong version!", appDependencies[i].name);
		}
		systems.Add(system);
	}
#endif

	Log_Info("Running application %s with %d systems", appName, systems.Size());
	s32 result = app->Run(systems);

	for (ssize i = 0; i < systems.Size(); i++)
	{
		delete systems[i];
	}

#ifndef CH_STATIC
	for (ssize i = 0; i < libs.Size(); i++)
	{
		delete libs[i];
	}
#endif

	Plat_Shutdown();
	Base_Shutdown();

	return result;
}
