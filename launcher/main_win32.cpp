/// \file Windows launcher
/// \copyright 2025 Randomcode Developers

/// Hacky code to load Launcher.dll from the bin directory next to the executable.
/// It can only use ntdll.dll, and only functions guaranteed to be on Windows XP and later (ntdll does include a subset of the
/// CRT's code).

#include "base/compiler.h"

#include "phnt_wrapper.h"

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

#ifdef CH_XENON
#pragma comment(linker, "/SUBSYSTEM:XBOX,2.00")
#elif defined CH_IA32
#ifdef __clang__
#pragma comment(linker, "/SUBSYSTEM:" SUBSYSTEM ",5.00")
#else
// MSVC silently defaults to 6.0 if you specify 5.00 or lower
#pragma comment(linker, "/SUBSYSTEM:" SUBSYSTEM ",5.01")
#endif
#else
#pragma comment(linker, "/SUBSYSTEM:" SUBSYSTEM ",5.02")
#endif

extern "C" ATTRIBUTE(dllimport) int _snwprintf(wchar_t* Buffer, size_t Size, const wchar_t* Format, ...);

#ifndef CH_STATIC
static NORETURN void Die(const wchar_t* msg, NTSTATUS status)
{
	DbgPrint("Fatal error: NTSTATUS 0x%08X %ls", status, msg);

	UNICODE_STRING messageStr = {};
	messageStr.Buffer = CONST_CAST(wchar_t*, msg);
	messageStr.Length = static_cast<WORD>(wcslen(msg) * sizeof(wchar_t));
	messageStr.MaximumLength = messageStr.Length + sizeof(wchar_t);

	wchar_t title[] = L"Fatal error!";
	UNICODE_STRING titleStr = RTL_CONSTANT_STRING(title);
	ULONG_PTR params[] = {
		reinterpret_cast<ULONG_PTR>(&messageStr), reinterpret_cast<ULONG_PTR>(&titleStr), MB_ABORTRETRYIGNORE | MB_ICONERROR,
		INFINITE};
	ULONG response = 0;
	NtRaiseHardError(
		HARDERROR_OVERRIDE_ERRORMODE | STATUS_SERVICE_NOTIFICATION, (ULONG)ARRAYSIZE(params), 0b0011, params,
		OptionAbortRetryIgnore, &response);

	NtTerminateProcess(NtCurrentProcess(), status);
	UNREACHABLE();
}

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
extern "C" int Base_RunMain(int (*main)());
#endif

#ifndef CH_XENON
#pragma comment(linker, "/ENTRY:LauncherEntry")

extern "C" void __stdcall LauncherEntry()
{
#ifdef CH_STATIC
	extern int LauncherMain();

	__security_init_cookie();
	RunGlobalConstructors();
	//__dyn_tls_init(nullptr, DLL_THREAD_ATTACH, nullptr);
	Base_Init();
	int result = Base_RunMain(LauncherMain);
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
	wchar_t launcherDllBuffer[] = L"Launcher.dll";
	UNICODE_STRING launcherDll = RTL_CONSTANT_STRING(launcherDllBuffer);
	NTSTATUS status = LdrLoadDll(binDir, nullptr, &launcherDll, &launcher);
	if (!NT_SUCCESS(status))
	{
		// TODO: figure out how to display an error
		if (triedSameDir)
		{
			Die(L"Couldn't find Launcher.dll!", status);
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
			Die(L"Couldn't get LauncherMain in Launcher.dll!", status);
	}

	void* base = nullptr;
	wchar_t baseName[] = L"Base.dll";
	UNICODE_STRING baseString = RTL_CONSTANT_STRING(baseName);
	status = LdrGetDllHandle(nullptr, nullptr, &baseString, &base);
	if (!NT_SUCCESS(status))
	{
			Die(L"Couldn't find Base.dll!", status);
	}

	void* runMainAddr = nullptr;
	char runMainName[] = "Base_RunMain";
	ANSI_STRING runMainString = RTL_CONSTANT_STRING(runMainName);
	status = LdrGetProcedureAddress(base, &runMainString, 0, &runMainAddr);

	int (*Base_RunMain)(int (*main)()) = reinterpret_cast<int (*)(int (*)())>(runMainAddr);
	int (*LauncherMain)() = reinterpret_cast<int (*)()>(launcherMainAddr);
	int result = Base_RunMain(LauncherMain);

	NtTerminateProcess(NtCurrentProcess(), result);
#endif
}
#endif
