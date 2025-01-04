#include <cstdlib>
#include <ctime>
#include <dlfcn.h>

#include <asm/errno.h>
#include <asm/fcntl.h>
#include <asm/unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/user.h>

#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"
#include "platform_linux.h"

dstr g_exeDir;

uptr g_errno;

s64 g_timeZoneOffset;

BASEAPI void Plat_Init()
{
	char exePath[1024];

	Base_SysCall(__NR_readlink, reinterpret_cast<uptr>("/proc/self/exe"), reinterpret_cast<uptr>(exePath), ARRAY_SIZE(exePath));

	ssize index = Base_StrFind(exePath, '/', true);
	g_exeDir = Base_StrClone(exePath, index);
	if (!g_exeDir)
	{
		Base_AbortSafe(ABORT_RELEVANT_ERROR, "Failed to get executable directory!");
	}

	s64 now = Plat_GetMilliseconds() / 1000;
	struct tm* local = localtime(&now);
	g_timeZoneOffset = local->tm_gmtoff;
}

BASEAPI void Plat_Shutdown()
{
}

s32 s_argc;
char** s_argv;

BASEAPI void Base_Internal_SetArgs(s32 argc, char* argv[])
{
	s_argc = argc;
	s_argv = argv;
}

BASEAPI void Plat_GetArgs(CVector<CString>& args)
{
	args.Resize(s_argc);
	// have to add one at a time to construct strings
	for (s32 i = 0; i < s_argc; i++)
	{
		args[i] = s_argv[i];
	}
}

// TODO: copy code from Purpl
BASEAPI cstr Plat_GetSystemDescription()
{
	return "Linux";
}

BASEAPI cstr Plat_GetHardwareDescription()
{
	// TODO: cpuid and /proc/meminfo
	return "unknown hardware";
}

ATTRIBUTE(naked) uptr Base_SysCall(uptr number, uptr arg1, uptr arg2, uptr arg3, uptr arg4, uptr arg5, uptr arg6)
{
#ifdef CH_AMD64
	__asm__ volatile("pushq %rbp\n"
					 "movq %rsp, %rbp\n"
					 "\n"
					 "movq %rdi, %rax\n"
					 "movq %rsi, %rdi\n"
					 "movq %rdx, %rsi\n"
					 "movq %rcx, %rdx\n"
					 "movq %r8, %r10\n" // R10 instead of RCX because syscall uses RCX for the return address
					 "movq %r9, %r8\n"
					 "mov 16(%rsp), %r9\n"
					 "\n"
					 "syscall\n"
					 "\n"
					 "leave\n"
					 "retq\n");
#endif
}

BASEAPI NORETURN void Base_AbortSafe(s32 error, cstr msg)
{
	// TODO: make a window pop up
	Plat_WriteConsole("Fatal error: ");
	Plat_WriteConsole(msg);
	Plat_WriteConsole("\n");
	if (error == ABORT_RELEVANT_ERROR)
	{
		error = g_errno;
	}
	Base_SysCall(__NR_exit_group, error);
	__builtin_unreachable();
}

bool Base_GetSystemMemory(ssize size)
{
	// Linked list nodes, can contain any size of allocation, but there's a limit to the number of OS allocations
	static LinkedNode_t<SystemAllocation_t> memoryNodes[64];

	// Maximize available space by rounding up to page size directly
	size = ALIGN(size, PAGE_SIZE);

	ASSERT_MSG(
		g_memInfo.allocations.Size() < ARRAY_SIZE(memoryNodes),
		"OS allocation nodes exhausted, increase the size of the memory nodes array");

	LinkedNode_t<SystemAllocation_t>* node = &memoryNodes[g_memInfo.allocations.Size()];
	node->data.size = size;

	node->data.memory = reinterpret_cast<void*>(Base_SysCall(
		__NR_mmap, reinterpret_cast<uptr>(nullptr), node->data.size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
	if (node->data.memory == MAP_FAILED)
	{
		return false;
	}

	g_memInfo.size += size;
	g_memInfo.allocations.Append(node);

	return true;
}

BASEAPI ILibrary* Base_LoadLibrary(cstr name)
{
	static const cstr DLL_EXT = ".so";

	Log_Debug("Loading library %s%s%s", g_exeDir, name, DLL_EXT);

	dstr filename = Base_StrFormat("%s%s%s", g_exeDir, name, DLL_EXT);
	if (!filename)
	{
		return nullptr;
	}

	void* base = dlopen(filename, RTLD_GLOBAL | RTLD_LAZY);
	Base_Free(filename);
	if (!base)
	{
		return nullptr;
	}

	return new CLinuxLibrary(name, base);
}

CLinuxLibrary::CLinuxLibrary(cstr name, void* base) : m_name(Base_StrClone(name)), m_base(base)
{
}

CLinuxLibrary::~CLinuxLibrary()
{
	if (!m_name)
	{
		Base_Free(m_name);
	}
	dlclose(m_base);
}

void* CLinuxLibrary::GetSymbol(cstr name)
{
	return dlsym(m_base, name);
}

BASEAPI bool Plat_ConsoleHasColor()
{
	// safe enough to assume, identifying the terminal is probably terrible
	return true;
}

BASEAPI void Plat_WriteConsole(cstr message)
{
	Base_SysCall(__NR_write, STDOUT_FD, reinterpret_cast<uptr>(message), Base_StrLength(message));
}

BASEAPI cstr Plat_GetSaveLocation()
{
	static char s_saveDir[1024];

	if (!Base_StrLength(s_saveDir))
	{
		if (Plat_GetEnvironment("XDG_USER_DATA_HOME") && Base_StrLength(Plat_GetEnvironment("XDG_USER_DATA_HOME")))
		{
			Base_StrFormat(s_saveDir, ARRAY_SIZE(s_saveDir), "%s" GAME_NAME "/", Plat_GetEnvironment("XDG_USER_DATA_HOME"));
		}
		else if (Plat_GetEnvironment("HOME") && Base_StrLength(Plat_GetEnvironment("HOME")))
		{
			Base_StrFormat(s_saveDir, ARRAY_SIZE(s_saveDir), "%s/.local/share/" GAME_NAME "/", Plat_GetEnvironment("HOME"));
		}
		else
		{
			// good enough
			Base_StrCopy(s_saveDir, "/tmp/" GAME_NAME "/", ARRAY_SIZE(s_saveDir));
		}
	}

	return s_saveDir;
}

BASEAPI cstr Plat_GetEnvironment(cstr name)
{
	return getenv(name);
}

BASEAPI u64 Plat_GetMilliseconds()
{
	struct timespec time = {};

	Base_SysCall(__NR_clock_gettime, CLOCK_REALTIME, reinterpret_cast<uptr>(&time));

	return time.tv_sec * 1000 + (time.tv_nsec + 500000) / 1000000;
}

/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400 * (31 + 29))

#define DAYS_PER_400Y (365 * 400 + 97)
#define DAYS_PER_100Y (365 * 100 + 24)
#define DAYS_PER_4Y   (365 * 4 + 1)

// https://git.musl-libc.org/cgit/musl/tree/src/time/__secs_to_tm.c
BASEAPI void Plat_GetDateTime(DateTime_t& time, bool utc)
{
	long long days, secs, years;
	int remdays, remsecs, remyears;
	int qc_cycles, c_cycles, q_cycles;
	int months;
	int wday, yday, leap;
	static const char days_in_month[] = {31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29};

	s64 millis = Plat_GetMilliseconds();
	s64 t = millis / 1000 + (utc ? 0 : g_timeZoneOffset);

	/* Reject time_t values whose year would overflow int */
	if (t < INT32_MIN * 31622400LL || t > INT32_MAX * 31622400LL)
		return;

	secs = t - LEAPOCH;
	days = secs / 86400;
	remsecs = secs % 86400;
	if (remsecs < 0)
	{
		remsecs += 86400;
		days--;
	}

	wday = (3 + days) % 7;
	if (wday < 0)
		wday += 7;

	qc_cycles = days / DAYS_PER_400Y;
	remdays = days % DAYS_PER_400Y;
	if (remdays < 0)
	{
		remdays += DAYS_PER_400Y;
		qc_cycles--;
	}

	c_cycles = remdays / DAYS_PER_100Y;
	if (c_cycles == 4)
		c_cycles--;
	remdays -= c_cycles * DAYS_PER_100Y;

	q_cycles = remdays / DAYS_PER_4Y;
	if (q_cycles == 25)
		q_cycles--;
	remdays -= q_cycles * DAYS_PER_4Y;

	remyears = remdays / 365;
	if (remyears == 4)
		remyears--;
	remdays -= remyears * 365;

	leap = !remyears && (q_cycles || !c_cycles);
	yday = remdays + 31 + 28 + leap;
	if (yday >= 365 + leap)
		yday -= 365 + leap;

	years = remyears + 4 * q_cycles + 100 * c_cycles + 400LL * qc_cycles;

	for (months = 0; days_in_month[months] <= remdays; months++)
		remdays -= days_in_month[months];

	if (months >= 10)
	{
		months -= 12;
		years++;
	}

	if (years + 100 > INT32_MAX || years + 100 < INT32_MIN)
		return;

	time.year = years + 2000;
	time.month = months + 3;
	time.day = remdays + 1;
	time.weekDay = wday;

	time.hour = remsecs / 3600;
	time.minute = remsecs / 60 % 60;
	time.second = remsecs % 60;
	time.millisecond = millis - (t - (utc ? 0 : g_timeZoneOffset)) * 1000;
}
