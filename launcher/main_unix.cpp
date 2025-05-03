/// \file Unix launcher
/// \copyright 2025 Randomcode Developers

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <dlfcn.h>
#include <linux/limits.h>

#include "base/platform.h"
#include "base/types.h"
#include "launcher.h"

static cstr GetEngineDir()
{
	static char s_directory[PATH_MAX + 1] = {0};

	if (!strlen(s_directory))
	{
		assert(readlink("/proc/self/exe", s_directory, ArraySize<usize>(s_directory)) > 0);
		dstr exeName = strrchr(s_directory, '/');
		*exeName = 0;
	}

	return s_directory;
}

extern "C" s32 Startup(s32 argc, dstr argv[], void (*DynamicAtExit)())
{
#ifdef CH_STATIC
	extern void Base_Internal_SetArgs(s32 argc, dstr argv[]);
	extern s32 LauncherMain();

	Base_Internal_SetArgs(argc, argv);
	Base_Init();
	s32 code = LauncherMain();
	Base_Shutdown();

	return code;
#else
	void (*Base_Internal_SetArgs)(s32 argc, dstr argv[]) = nullptr;
	s32 (*LauncherMain)() = nullptr;

	extern dstr* environ;

	// put new paths in, then restart with updated environment
	if (!getenv("LIBPATH_UPDATED"))
	{
		char libPath[PATH_MAX + 1] = {0};
		dstr envLibPath = getenv("LD_LIBRARY_PATH");
		snprintf(libPath, ArraySize<s32>(libPath), "%s/bin/:%s/%s%s", GetEngineDir(), GetEngineDir(), envLibPath ? ":" : "", envLibPath ? envLibPath : "");
		setenv("LD_LIBRARY_PATH", libPath, true);
		setenv("LIBPATH_UPDATED", "1", true);
		printf("Restarting with LD_LIBRARY_PATH=\"%s\"\n", libPath);
		execve(argv[0], argv, environ);
	}

	void* launcher = dlopen("libLauncher.so", RTLD_NOW);
	if (!launcher)
	{
		printf("Failed to load libLauncher.so with LD_LIBRARY_PATH=%s: %s\n", getenv("LD_LIBRARY_PATH"), dlerror());
		exit(errno);
	}

	// Base.so should be loaded by Launcher.so
	void* base = dlopen("libBase.so", RTLD_NOW);
	if (!base)
	{
		printf("Failed to get libBase.so handle: %s\n", dlerror());
		exit(errno);
	}

	*reinterpret_cast<void**>(&Base_Internal_SetArgs) = dlsym(base, "Base_Internal_SetArgs");
	*reinterpret_cast<void**>(&LauncherMain) = dlsym(launcher, "LauncherMain");

	Base_Internal_SetArgs(argc, argv);
	s32 code = LauncherMain();

	if (DynamicAtExit)
	{
		DynamicAtExit();
	}

	return code;
#endif
}

extern "C" int main(int argc, char* argv[])
{
	return Startup(argc, argv, nullptr);
}

#if 0
ATTRIBUTE(naked) extern "C" void _start()
{
#ifdef CH_AMD64
	__asm__("xorl %%ebp, %%ebp\n"
			"movq %%rdx, %%rdx\n"            // dynamic linker at exit thingy
			"popq %%rdi\n"                   // argc
			"movq %%rsp, %%rsi\n"            // argv
			"xorl %%eax, %%eax\n"            // abi wants this or something
			"and $0xfffffffffffffff0, %%rsp\n" // align stack to 16
			"call %P0"
			:
			: "i"(Startup)
			: "memory", "%rax", "%rbx", "%rdx");
#endif
}
#endif
