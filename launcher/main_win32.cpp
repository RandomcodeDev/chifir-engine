#include "base/platform.h"
#include "base/types.h"
#include "launcher.h"

#ifndef CH_DEBUG
// hinting the nvidia driver to use the dedicated graphics card in an optimus
// configuration for more info, see:
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
ATTRIBUTE(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// same thing for AMD GPUs using v13.35 or newer drivers
ATTRIBUTE(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

#ifdef CH_XBOX360
#pragma comment(linker, "/SUBSYSTEM:XBOX,2.00")
#elif defined CH_IA32
#pragma comment(linker, "/SUBSYSTEM:WINDOWS,5.01")
#else
#pragma comment(linker, "/SUBSYSTEM:WINDOWS,5.02")
#endif

extern "C"
{
#ifndef CH_XBOX360
	NORETURN void __stdcall WinMainCRTStartup()
	{
		__security_init_cookie();

		LauncherMain();

		NtTerminateProcess(NtCurrentProcess(), STATUS_SUCCESS);
	}
#endif
}
