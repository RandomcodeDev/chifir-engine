#include "platform_linux.h"
#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"

static u64 s_errno;

u64 Base_SysCall(u64 number, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6)
{
    u64 retVal = 0;
    do
    {
#ifdef CH_AMD64
	// RCX is the syscall number, so R10 is used
	__asm__ volatile("mov %4, %%r10\n"
					 "mov %5, %%r8\n"
					 "mov %6, %%r9\n"
					 "syscall\n"
                     "mov %%rax, $0\n"
					 : "=a"(retVal)
					 : "c"(number), "D"(arg1), "S"(arg2), "d"(arg3), "g"(arg4), "g"(arg5), "g"(arg6)
                     : "%r8", "%r9", "%r10");
#endif
    } while (retVal == EINTR);

    s_errno = retVal;

    return retVal;
}

BASEAPI NORETURN void Base_AbortSafe(s32 error, cstr msg)
{
    // TODO: make a window pop up
    Plat_WriteConsole("Fatal error: ");
    Plat_WriteConsole(msg);
    Plat_WriteConsole("\n");
    if (error == 1)
    {
        error = s_errno;
    }
    Base_SysCall(__NR_exit_group, error);
    __builtin_unreachable();
}

BASEAPI void Plat_WriteConsole(cstr message)
{
    Base_SysCall(__NR_write, STDOUT_FD, message, Base_StrLength(message));
}
