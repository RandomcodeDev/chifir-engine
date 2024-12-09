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
		__asm__("xor %%ebp, %%ebp\n"
				"mov %%rdx, $3"              // dynamic linker at exit thingy
				"mov (%%rsp), $0\n"          // argc
				"mov 8(%%rsp), $1\n"         // argv
				"mov 16(%%rsp, %0, 8), $2\n" // envp
				"xor %%eax, %%eax\n"         // abi wants this or something
				"callq %0\n"                 // call LauncherMain
				"mov %%eax, %%ebx\n"         // save return value
				"callq $3\n"                 // call dynamic linker thing
				"mov %%ecx, %1\n"            // exit_group
				"mov %%edi, %%ebx\n"         // return value of LauncherMain
				"syscall\n"                  // exit the process
				: "+r"(s_argc), "+r"(s_argv), "+r"(s_envp), "+r"(DynamicAtExit)
				: "i"(LauncherMain), "i"(__NR_exit_group)
				: "memory", "%rbx");
#endif
	}
}
