#include <ctime>

#include <asm/errno.h>
#include <asm/fcntl.h>
#include <asm/unistd.h>

#include <sys/mman.h>
#include <sys/time.h>
#include <sys/user.h>

#include "platform_linux.h"
#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"

uptr g_errno;

BASEAPI void Plat_Init()
{
}

BASEAPI void Plat_Shutdown()
{
}

// TODO: copy code from Purpl
BASEAPI cstr Plat_GetSystemDescription()
{
	return "Linux";
}

BASEAPI cstr Plat_GetHardwareDescription()
{
	return "unknown hardware";
}

uptr Base_SysCall(uptr number, uptr arg1, uptr arg2, uptr arg3, uptr arg4, uptr arg5, uptr arg6)
{
	uptr retVal = 0;
	do
	{
#ifdef CH_AMD64
		// RCX is the syscall number, so R10 is used
		__asm__ volatile("mov %4, %%r10\n"
						 "mov %5, %%r8\n"
						 "mov %6, %%r9\n"
						 "syscall\n"
						 : "=a"(retVal)
						 : "c"(number), "D"(arg1), "S"(arg2), "d"(arg3), "g"(arg4), "g"(arg5), "g"(arg6)
						 : "%r8", "%r9", "%r10");
#endif
	} while (retVal == EINTR);

	g_errno = retVal;

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

	Base_SysCall(__NR_mmap, reinterpret_cast<uptr>(&node->data.memory), node->data.size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (g_errno != 0)
    {
        return false;
    }

	g_memInfo.size += size;
	g_memInfo.allocations.Append(node);

    return true;
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

BASEAPI u64 Plat_GetMilliseconds()
{
	struct timespec time = {};

	Base_SysCall(__NR_clock_gettime, CLOCK_REALTIME, reinterpret_cast<uptr>(&time));

	return time.tv_sec * 1000 + (time.tv_nsec + 500000) / 1000000;
}

// musl libc code from https://git.musl-libc.org/cgit/musl/tree/src/time/__secs_to_tm.c
/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400 * (31 + 29))

#define DAYS_PER_400Y (365 * 400 + 97)
#define DAYS_PER_100Y (365 * 100 + 24)
#define DAYS_PER_4Y   (365 * 4 + 1)

static bool UnixTimestampToDateTime(s64 t, DateTime_t& time)
{
	long long days, secs, years;
	int remdays, remsecs, remyears;
	int qc_cycles, c_cycles, q_cycles;
	int months;
	int wday;
	static const char days_in_month[] = {31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29};

	/* Reject time_t values whose year would overflow int */
	if (t < INT32_MIN * 31622400LL || t > INT32_MAX * 31622400LL)
		return false;

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

	years = remyears + 4 * q_cycles + 100 * c_cycles + 400LL * qc_cycles;

	for (months = 0; days_in_month[months] <= remdays; months++)
		remdays -= days_in_month[months];

	if (months >= 10)
	{
		months -= 12;
		years++;
	}

	if (years + 100 > INT32_MAX || years + 100 < INT32_MIN)
		return false;

	time.year = years + 100;
	time.month = months + 2;
	time.day = remdays + 1;
	time.weekDay = wday;

	time.hour = remsecs / 3600;
	time.minute = remsecs / 60 % 60;
	time.second = remsecs % 60;

	return true;
}

BASEAPI void Plat_GetDateTime(DateTime_t& time, bool utc)
{
	u64 timeZoneOffset = 0;
	UnixTimestampToDateTime(Plat_GetMilliseconds() / 1000 + (utc ? 0 : timeZoneOffset), time);
}
