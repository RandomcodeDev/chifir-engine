/// \file Launcher implementation
/// \copyright 2025 Randomcode Developers

#include "base/filesystem.h"
#include "base/loader.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"

#include "rendersystem/irendersystem.h"

#include "utility/utility.h"

#include "videosystem/ivideosystem.h"

#include "iapplication.h"
#include "isystem.h"
#include "launcher.h"

#ifdef CH_STATIC
extern IApplication* CreateEngine();
#else
static IApplication* GetApplication(ILibrary* library)
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

	Log_Info("Got %s v%d", system->GetName(), system->GetVersion());

	return system;
}
#endif

extern "C" LAUNCHERAPI s32 LauncherMain()
{
	CVector<CString> args;
	Plat_GetArgs(args);

#ifdef CH_STATIC
	cstr appName = "Engine";

	CVector<ISystem*> systems;
	systems.Add(CreateVideoSystem());
	systems.Add(CreateRenderSystem());

	IApplication* app = CreateEngine();

	app->Setup(args);
#else
	cstr appName = "Engine";

	// TODO: add proper argument parsing
	for (ssize i = 0; i < args.Size(); i++)
	{
		if (args[i] == "-app")
		{
			if (i + 1 < args.Size())
			{
				i++;
				appName = args[i].Data();
			}
			else
			{
				Base_Quit("Missing argument to -app");
			}
		}
	}

	// first, try bin folder. if that fails (such as when running from the build directory), try same as exe.

	Log_Debug("Loading application %s", appName);
	ILibrary* appLib = Base_LoadEngineLibrary(appName);
	if (!appLib)
	{
		Base_Quit("Failed to load application %s!", appName);
	}

	Log_Debug("Initializing application %s", appName);
	IApplication* app = GetApplication(appLib);
	if (!app)
	{
		Base_Quit("Failed to initialize application %s!", appName);
	}

	app->Setup(args);

	CVector<SystemDependency> appSystems;
	CVector<LibDependency_t> appLibs;
	app->GetDependencies(appSystems, appLibs);

	Log_Debug("Loading libraries for application %s", appName);
	CVector<ILibrary*> libs;
	for (ssize i = 0; i < appLibs.Size(); i++)
	{
		Log_Debug("Loading %s", appLibs[i].name);
		ILibrary* lib = Base_LoadEngineLibrary(appLibs[i].name);
		if (!lib)
		{
			if (appLibs[i].required)
			{
				Base_Quit("Failed to load required library %s!", appLibs[i].name);
			}
			else
			{
				Log_Error("Failed to load library %s", appLibs[i].name);
			}
		}
		else
		{
			libs.Add(lib);
		}
	}

	Log_Debug("Loading systems for application %s", appName);
	CVector<ISystem*> systems;
	for (ssize i = 0; i < appSystems.Size(); i++)
	{
		Log_Debug(
			"Loading %s, %sversion %u (%s)", appSystems[i].name, appSystems[i].requireExactVersion ? "" : "minimum ",
			appSystems[i].minimumVersion, appSystems[i].required ? "required" : "optional");
		ILibrary* lib = Base_LoadEngineLibrary(appSystems[i].name);
		if (!lib)
		{
			if (appSystems[i].required)
			{
				Base_Quit("Failed to load required system %s!", appSystems[i].name);
			}
			else
			{
				Log_Error("Failed to load system %s", appSystems[i].name);
			}
		}
		else
		{
			libs.Add(lib);
		}

		if (lib)
		{
			Log_Debug("Getting interface for %s", appSystems[i].name);
			ISystem* system = GetSystem(lib, appSystems[i].minimumVersion, appSystems[i].requireExactVersion);
			if (!system)
			{
				if (appSystems[i].required)
				{
					Base_Quit("Failed to get interface for %s, or it's the wrong version!", appSystems[i].name);
				}
				else {}
			}
			systems.Add(system);
		}
		else
		{
			systems.Add(nullptr);
		}
	}
#endif

	Log_Info("Running application %s with %d systems", appName, systems.Size());
	s32 result = app->Run(systems);

	for (ssize i = 0; i < systems.Size(); i++)
	{
		if (systems[i])
		{
			Log_Trace("Deleting system %d (%s v%u)", i, systems[i]->GetName(), systems[i]->GetVersion());
			delete systems[i];
		}
	}

#ifndef CH_STATIC
	Log_Trace("Deleting libraries");
	for (ssize i = 0; i < libs.Size(); i++)
	{
		if (libs[i])
		{
			delete libs[i];
		}
	}
#endif

	Plat_Shutdown();
	Base_Shutdown();

	return result;
}
