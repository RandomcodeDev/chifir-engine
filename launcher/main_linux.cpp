#include <asm/unistd.h>

#include "base/platform.h"
#include "base/types.h"
#include "launcher.h"

static s32 s_argc;
static char** s_argv;
static char* s_envp;
static void (*DynamicAtExit)();

extern "C"
{
	ATTRIBUTE(naked) void _start()
	{
#ifdef CH_AMD64
		__asm__("xorl %%ebp, %%ebp\n"
				"movq %%rdx, %3\n"            // dynamic linker at exit thingy
				"movl (%%rsp), %0\n"          // argc
				"movq 8(%%rsp), %1\n"         // argv
				"movq 16(%%rsp, %1, 8), %2\n" // envp
				"xorl %%eax, %%eax\n"         // abi wants this or something
				"call %P4\n"                  // call LauncherMain
				"movl %%ebx, %%eax\n"         // save return value
				"call %P6\n"                  // call dynamic linker thing
				"movq %5, %%rax\n"            // exit_group
				"movl %%ebx, %%edi\n"         // return value of LauncherMain
				"syscall\n"                   // exit the process
				: "+r"(s_argc), "+r"(s_argv), "+r"(s_envp), "+r"(DynamicAtExit)
				: "m"(LauncherMain), "i"(__NR_exit_group), "m"(DynamicAtExit)
				: "memory", "%rbx");
#endif
	}
}
