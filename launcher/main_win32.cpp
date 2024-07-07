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

extern "C"
{
#ifdef CH_XBOX360
	NORETURN void __stdcall mainCRTStartup()
#else
	NORETURN void __stdcall WinMainCRTStartup()
#endif
	{
#if _MSC_VER >= 1700
		__security_init_cookie();
#endif

		LauncherMain();

		NtTerminateProcess(NtCurrentProcess(), STATUS_SUCCESS);
	}
}
