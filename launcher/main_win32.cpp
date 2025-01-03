#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"
#include "launcher.h"

#ifndef CH_DEBUG
// hinting the nvidia driver to use the dedicated graphics card in an optimus
// configuration for more info, see:
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
ATTRIBUTE(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// same thing for AMD GPUs using v13.35 or newer drivers
ATTRIBUTE(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

#ifndef CH_RETAIL
#define SUBSYSTEM "CONSOLE"
#else
#define SUBSYSTEM "WINDOWS"
#endif

#ifdef CH_XBOX360
#pragma comment(linker, "/SUBSYSTEM:XBOX,2.00")
#elif defined CH_IA32
#pragma comment(linker, "/SUBSYSTEM:" SUBSYSTEM ",5.01")
#else
#pragma comment(linker, "/SUBSYSTEM:" SUBSYSTEM ",5.02")
#endif

extern "C"
{
#ifndef CH_XBOX360
#ifndef CH_RETAIL
	void mainCRTStartup()
#else
	void __stdcall WinMainCRTStartup()
#endif
	{
		__security_init_cookie();

		RunGlobalConstructors();
		LauncherMain();
		RunGlobalDestructors();

		NtTerminateProcess(NtCurrentProcess(), STATUS_SUCCESS);
	}
#endif
}
