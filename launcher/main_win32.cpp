#include "base/types.h"
#include "launcher.h"
#include "platform/platform.h"

#ifndef KR_DEBUG
// hinting the nvidia driver to use the dedicated graphics card in an optimus
// configuration for more info, see:
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// same thing for AMD GPUs using v13.35 or newer drivers
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

__declspec(noreturn) void __cdecl WinMainCRTStartup()
{
	s32 ret = LauncherMain();

	NtTerminateProcess(NtCurrentProcess(), ret);
}
