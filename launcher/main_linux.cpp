#include <asm/unistd.h>
#include <asm/unistd_64.h>

#include "base/platform.h"
#include "base/types.h"
#include "launcher.h"

static s32 s_argc;
static char** s_argv;
static char* s_envp;
static void (*DynamicAtExit)();

extern "C"
{
	void Startup()
	{
		s32 code = LauncherMain();

		if (DynamicAtExit)
		{
			DynamicAtExit();
		}

		// Base_SysCall is limited to Base.so
		__asm__("movl %1, %%ecx\n"
				"movl %0, %%edi\n"
				"syscall\n"
				:
				: "r"(code), "i"(__NR_exit_group));
	}

	ATTRIBUTE(naked) void _start()
	{
#ifdef CH_AMD64
		__asm__("xorl %%ebp, %%ebp\n"
				"movq %%rdx, %3\n"            // dynamic linker at exit thingy
				"movl (%%rsp), %%ecx\n"          // argc
				"movl %%ecx, %1\n"
				"movq 8(%%rsp), %%rdx\n"         // argv
				"movq %%rdx, %2\n"
				"movq 16(%%rsp, %%rcx, 8), %%rcx\n" // envp
				"movq %%rcx, %3\n"
				"xorl %%eax, %%eax\n"         // abi wants this or something
				"call %P4"
				: "+m"(s_argc), "+m"(s_argv), "+m"(s_envp), "+m"(DynamicAtExit)
				: "i"(Startup)
				: "memory", "%rax", "%rbx", "%rdx");
#endif
	}
}
