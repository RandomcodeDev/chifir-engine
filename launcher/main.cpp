#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"
#include "iapplication.h"
#include "isystem.h"
#include "launcher.h"
#include "utility/log.h"
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

// ChatGPT generated allocator test, so far has shown that it works but the size of the coalesced free list doesn't quite add up
void TestAlloc()
{
	const int NumIterations = 500000; // Number of allocation/deallocation cycles
	const int AllocationSize = 64;    // Size of each allocation in bytes

	void** allocations = new void*[NumIterations];

	// Allocate memory in a loop
	for (int i = 0; i < NumIterations; ++i)
	{
		void* ptr = Base_Alloc(AllocationSize);
		if (!ptr)
		{
			Base_Quit(1, "Allocation %d failed", i);
		}

		allocations[i] = ptr;
	}

	// Free all allocated memory
	for (int i = 0; i < NumIterations; ++i)
	{
		Base_Free(allocations[i]);
	}

	delete[] allocations; // Free the array of pointers
}

struct TestStruct
{
	int value;
	TestStruct() : value(5)
	{
	}
	TestStruct(int v) : value(v)
	{
	}
};

// ChatGPT CVector test
void TestVector()
{
	CVector<TestStruct> vec;
	ASSERT(vec.Size() == 0);

	// Test adding elements
	vec.Add(TestStruct(1));
	vec.Add(TestStruct(2));
	vec.Add(TestStruct(3));
	ASSERT(vec.Size() == 3);

	// Test accessing elements
	ASSERT(vec[0].value == 1);
	ASSERT(vec[1].value == 2);
	ASSERT(vec[2].value == 3);

	// Test resizing
	vec.Resize(5);
	ASSERT(vec.Size() == 5);

	// Test reserve
	vec.Reserve(10);
	ASSERT(vec.Size() == 5); // Size should remain the same

	// Test deleting an element
	vec.Delete(1);
	ASSERT(vec.Size() == 4);
	ASSERT(vec[1].value == 3);
}

class CDbgPrintLogWriter : public ILogWriter
{
  public:
	void Write(const LogMessage_t& message)
	{
		static const cstr LEVEL_NAMES[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

		if (message.isAddress)
		{
			DbgPrint(
				"[%s] [0x%llX@%s %s] %s\n", LEVEL_NAMES[message.level], message.location, message.file, message.function,
				message.message);
		}
		else
		{
			DbgPrint(
				"[%s] [%s:%d %s] %s\n", LEVEL_NAMES[message.level], message.file, message.location, message.function,
				message.message);
		}
	}
};

extern "C" LAUNCHERAPI s32 LauncherMain()
{
#ifdef CH_XBOX360
	__security_init_cookie();
#endif

	Base_Init();
	Plat_Init();

	Log_AddWriter(new CDbgPrintLogWriter());

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
		Util_Fatal("Failed to load application %s!", appName);
	}

	Log_Info("Initializing application %s", appName);
	CVector<SystemDependency_t> appDependencies;
	IApplication* app = GetApplication(appLib, appDependencies);
	if (!app)
	{
		Util_Fatal("Failed to initialize application %s!", appName);
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
			Util_Fatal("Failed to load system %s!", appDependencies[i].name);
		}
		libs.Add(lib);

		Log_Info("Getting interface for %s", appDependencies[i].name);
		ISystem* system = GetSystem(lib, appDependencies[i].minimumVersion, appDependencies[i].requireExactVersion);
		if (!system)
		{
			Util_Fatal("Failed to get interface for %s, or it's the wrong version!", appDependencies[i].name);
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
