#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"
#include "isystem.h"
#include "launcher.h"
#include "videosystem/ivideosystem.h"

// ChatGPT generated allocator test, so far has shown that it works but is much slower than the normal vcruntime malloc
void TestAlloc()
{
	const int NumIterations = 500000; // Number of allocation/deallocation cycles
	const int AllocationSize = 64;   // Size of each allocation in bytes

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

void TestVideoSystem()
{
	ILibrary* videoSystemLib = Base_LoadLibrary("VideoSystem");
	if (!videoSystemLib)
	{
		Base_QuitSafe(1, "Failed to load video system DLL");
	}

	IVideoSystem* videoSystem = static_cast<IVideoSystem*>(GetSystem(videoSystemLib, IVideoSystem::VERSION));
	if (!videoSystem)
	{
		Base_Quit(1, "Failed to create video system interface with version %u or greater", IVideoSystem::VERSION);
	}

	if (!videoSystem->Initialize())
	{
		Base_QuitSafe(1, "Failed to initialize video system");
	}

	while (videoSystem->Update())
	{

	}

	videoSystem->Shutdown();
	delete videoSystem;
	delete videoSystemLib;
}

extern "C" LAUNCHERAPI s32 LauncherMain()
{
#ifdef CH_XBOX360
	__security_init_cookie();
#endif

	Base_Init();
	Plat_Init();

	TestAlloc();
	TestAlloc();
	TestVideoSystem();

	Plat_Shutdown();
	Base_Shutdown();

	return 0;
}
