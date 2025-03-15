// Hacky code to load Launcher.dll from the bin directory next to the executable.
// It can only use ntdll.dll

#define wcsrchr _wcsrchr
#define wcsncpy _wcsncpy
#define wcslen _wcslen
#include "base/compiler.h"
#include "phnt_wrapper.h"
#undef wcsrchr
#undef wcsncpy
#undef wcslen

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

extern "C" __declspec(dllimport) int _snwprintf(wchar_t* Buffer, size_t BufferCount, const wchar_t* Format, ...);
extern "C" __declspec(dllimport) size_t wcslen(const wchar_t* Buffer);
extern "C" __declspec(dllimport) wchar_t* wcsncpy(wchar_t* Destination, const wchar_t* Source, size_t Count);
extern "C" __declspec(dllimport) const wchar_t* wcsrchr(const wchar_t *str, wchar_t c);

#ifndef CH_STATIC
static PCWSTR GetEngineDir()
{
	static wchar_t s_directory[MAX_PATH + 1] = {0};

	if (!wcslen(s_directory))
	{
		PUNICODE_STRING imagePath = &NtCurrentPeb()->ProcessParameters->ImagePathName;
		PCWSTR imageName = wcsrchr(imagePath->Buffer, L'\\');
		wcsncpy(s_directory, imagePath->Buffer, imageName - imagePath->Buffer);
	}

	return s_directory;
}
#else
extern void Base_Init();
extern void Base_Shutdown();
#endif

extern "C"
#ifndef CH_XBOX360
#ifndef CH_RETAIL
	void
	mainCRTStartup()
#else
	void __stdcall WinMainCRTStartup()
#endif
{
#ifdef CH_STATIC
	extern int LauncherMain();

	__security_init_cookie();
	RunGlobalConstructors();
	Base_Init();
	int result = LauncherMain();
	Base_Shutdown();
	RunGlobalConstructors();

	NtTerminateProcess(NtCurrentProcess(), result);
#else
	PCWSTR engineDir = GetEngineDir();
	wchar_t binDir[MAX_PATH + 1];
	_snwprintf(binDir, ARRAYSIZE(binDir), L"%s\\bin", engineDir);
	void* launcher = nullptr;
	bool triedSameDir = false;
Load:
	UNICODE_STRING launcherDll = RTL_CONSTANT_STRING(L"Launcher.dll");
	NTSTATUS status = LdrLoadDll(binDir, nullptr, &launcherDll, &launcher);
	if (!NT_SUCCESS(status))
	{
		// TODO: figure out how to display an error
		if (triedSameDir)
		{
			NtTerminateProcess(NtCurrentProcess(), status);
		}
		else
		{
			triedSameDir = true;
			_snwprintf(binDir, ARRAYSIZE(binDir), L"%s", engineDir);
			goto Load;
		}
	}

	void* launcherMainAddr = nullptr;
	char launcherMainName[] = "LauncherMain";
	ANSI_STRING launcherMainString = RTL_CONSTANT_STRING(launcherMainName);
	status = LdrGetProcedureAddress(launcher, &launcherMainString, 0, &launcherMainAddr);
	if (!NT_SUCCESS(status))
	{
		NtTerminateProcess(NtCurrentProcess(), status);
	}

	int (*LauncherMain)() = reinterpret_cast<int (*)()>(launcherMainAddr);
	int result = LauncherMain();

	NtTerminateProcess(NtCurrentProcess(), result);
#endif
}
#endif
