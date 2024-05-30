#include "framework/CCommandLine.h"
#include "framework/Framework.h"
#include "framework/IApplication.h"
#include "framework/IPlatform.h"
#include "framework/ISharedLibrary.h"
#include "framework/ISystem.h"
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

	SetupLogging();

	GetPlatform()->Initialize();

	std::string appName = "Engine";
	if (cmdLine.HasArg("app"))
	{
		appName = cmdLine.GetArg("app");
	}

	// note: raw pointers are mainly used to keep DLL interfaces clean

	std::shared_ptr<ISharedLibrary> appLib = GetPlatform()->LoadLibrary(appName);
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

	std::string logDir = fmt::format("{}/{}/{}", GetPlatform()->GetUserDataPath(), app->GetOrganization(), app->GetName());
	GetPlatform()->CreateDirectory(logDir);
	std::string logPath = fmt::format("{}/{}_{:%Y-%m-%d_%H-%M-%S}.log", logDir, app->GetIdName(), std::chrono::system_clock::now());
	AddLogFile(logPath);

	KR_LOG_INFO("Loaded app {} ({}) by {} on {}", app->GetName(), app->GetIdName(), app->GetOrganization(), GetPlatform()->DescribeOs());

	KR_LOG_INFO("Loading systems");
	std::vector<SystemDependency> systemDeps = app->GetSystems();
	std::vector<std::shared_ptr<ISharedLibrary>> systemLibs;
	systemLibs.reserve(systemDeps.size());
	std::unordered_map<std::string, ISystem*> systems;
	systems.reserve(systemDeps.size());
	for (auto& dep : systemDeps)
	{
		std::shared_ptr<ISharedLibrary> lib = GetPlatform()->LoadLibrary(dep.name + "System");
		if (!lib && !dep.optional)
		{
			KR_QUIT("Failed to load required system {}!", dep.name);
		}

		systemLibs.push_back(lib);

		auto getVersion = lib->GetFunction<u32>("GetSystemVersion");
		if (!getVersion)
		{
			KR_QUIT("Unable to get system version!");
		}

		u32 version = getVersion();
		if ((version < dep.minimumVersion) || (dep.requireExactVersion && version != dep.minimumVersion))
		{
			KR_QUIT(
				"Version mismatch! Version {}{} is required, but present version is {}!", dep.minimumVersion,
				dep.requireExactVersion ? "" : " or higher", version);
		}

		auto createInterface = lib->GetFunction<ISystem*>("CreateInterface");
		if (!createInterface && !dep.optional)
		{
			KR_QUIT("Failed to get interface!");
		}

		ISystem* system = createInterface();
		if (!system && !dep.optional)
		{
			KR_QUIT("Failed to create interface!");
		}
		systems[dep.name] = system;
	}

	app->Run(cmdLine, systems);

	for (auto pair : systems)
	{
		if (pair.second)
		{
			delete pair.second;
		}
	}

	systemLibs.clear();

	delete app;
	appLib.reset();

	GetPlatform()->Shutdown();

	return 0;
}
