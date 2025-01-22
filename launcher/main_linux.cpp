#include <asm/unistd.h>
#include <asm/unistd_64.h>
#include <dlfcn.h>

#include "base/platform.h"
#include "base/types.h"
#include "launcher.h"

void (*Base_Internal_SetArgs)(s32 argc, char* argv[]);

void Startup(s32 argc, char* argv[], void (*DynamicAtExit)())
{
	char** envp = argv + argc + 1;
	(void)envp;

	// TODO: dlopen everything

	Base_Internal_SetArgs(argc, argv);
	s32 code = LauncherMain();

	if (DynamicAtExit)
	{
		DynamicAtExit();
	}

	// Base_SysCall is internal to Base.so
	__asm__("movl %1, %%ecx\n"
			"movl %0, %%edi\n"
			"syscall\n"
			:
			: "r"(code), "i"(__NR_exit_group));
}

extern "C" int main(int argc, char* argv[])
{
	Startup(argc, argv, nullptr);
	return 0;
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
