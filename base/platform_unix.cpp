/// \file Unix support
/// \copyright 2025 Randomcode Developers

#include "base/macros.h"
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <dlfcn.h>

#ifdef CH_LINUX
#include <linux/limits.h>
#endif

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/user.h>
#include <unistd.h>

#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"
#include "platform_unix.h"

dstr g_exeDir;

s64 g_timeZoneOffset;

static void SignalHandler(s32 signal, siginfo_t* sigInfo, void* context)
{
	cstr baseError = "Unknown error";
	cstr error = "unknown error";

	// return if things can continue, otherwise break and quit with the error

	auto ctx = reinterpret_cast<ucontext_t*>(context);
	auto pc =
#ifdef CH_AMD64
		ctx->uc_mcontext.gregs[REG_RIP];
#elif defined CH_IA32
		ctx->uc_mcontext.gregs[REG_EIP];
#elif defined CH_ARM64
		ctx->uc_mcontext.gregs[REG_PC];
#endif

	cstr type = nullptr;

	switch (signal)
	{
	case SIGILL:
		baseError = "Illegal operation";
		type = "executing";
		switch (sigInfo->si_code)
		{
		case ILL_ILLOPC:
			error = "illegal opcode";
			break;
		case ILL_ILLOPN:
			error = "illegal operand";
			break;
		case ILL_ILLADR:
			error = "illegal addressing mode";
			break;
		case ILL_ILLTRP:
			error = "illegal trap";
			break;
		case ILL_PRVOPC:
			error = "privileged opcode";
			break;
		case ILL_PRVREG:
			error = "privileged register";
			break;
		case ILL_COPROC:
			error = "coprocessor error";
			break;
		case ILL_BADSTK:
			error = "internal stack error";
			break;
		}
		break;
	case SIGSEGV:
		baseError = "Segmentation fault";
		type = "access";
		switch (sigInfo->si_code)
		{
		case SEGV_MAPERR:
			error = "address not mapped to object";
			break;
		case SEGV_ACCERR:
			error = "invalid permissions for mapped object";
			break;
#ifdef CH_LINUX
		case SEGV_BNDERR:
			error = "failed bounds checks";
			break;
		case SEGV_PKUERR:
			error = "access was denied by memory protection keys";
			break;
#endif
		}
		break;
	case SIGFPE:
		baseError = "Arithmetic error";
		switch (sigInfo->si_code)
		{
		case FPE_INTDIV:
			error = "integer divide by zero";
			break;
		case FPE_INTOVF:
			error = "integer overflow";
			break;
		case FPE_FLTDIV:
			error = "floating-point divide by zero";
			break;
		case FPE_FLTOVF:
			error = "floating-point overflow";
			break;
		case FPE_FLTUND:
			error = "floating-point underflow";
			break;
		case FPE_FLTRES:
			error = "floating-point inexact result";
			break;
		case FPE_FLTINV:
			error = "floating-point invalid operation";
			break;
		case FPE_FLTSUB:
			error = "subscript out of range";
			break;
		}
		break;
	case SIGBUS:
		baseError = "Bus error";
		switch (sigInfo->si_code)
		{
		case BUS_ADRALN:
			error = "invalid address alignment";
			break;
		case BUS_ADRERR:
			error = "nonexistent physical address";
			break;
		case BUS_OBJERR:
			error = "object-specific hardware error";
			break;
		case BUS_MCEERR_AR:
			error = "hardware memory error consumed on a machine check; action required";
			break;
		case BUS_MCEERR_AO:
			error = "hardware memory error detected in process but not consumed; action optional";
			break;
		}
		break;
	case SIGINT:
	case SIGQUIT:
	case SIGTERM:
		Log_Info("Received quit signal %d, errno %d", signal, sigInfo->si_errno);
		g_quitSignalled = true;
		return;
	case SIGTRAP:
		Log_Info("Debug trap pc=0x%llX, errno %d", static_cast<u64>(pc), sigInfo->si_errno);
		return;
	default:
		Log_Info("Received signal %d", signal);
		return;
	}

	// doesn't use libc, so it's fine here
	Base_Quit(
		"%s pc=0x%llX addr=0x%llX%s%s%s: error %d: %s (si_code %d)", baseError, static_cast<u64>(pc), reinterpret_cast<u64>(sigInfo->si_addr),
		type ? " (" : "", type ? type : "", type ? ")" : "", sigInfo->si_errno, error, sigInfo->si_code);
}

BASEAPI void Plat_Init()
{
	if (!g_platInitialized)
	{
		char exePath[1024];

		ASSERT_MSG_SAFE(readlink("/proc/self/exe", exePath, ArraySize<usize>(exePath)) > 0, "Failed to get executable path!");

		ssize index = Base_StrFind(exePath, '/', true);
		g_exeDir = Base_StrClone(exePath, index);
		if (!g_exeDir)
		{
			Base_AbortSafe(ABORT_RELEVANT_ERROR, "Failed to get executable directory!");
		}

		time_t now = Plat_GetMilliseconds() / 1000;
		struct tm* local = localtime(&now);
		g_timeZoneOffset = local->tm_gmtoff;

		t_isMainThread = true;

		struct sigaction sigAction = {};
		sigAction.sa_flags = SA_SIGINFO;
		sigAction.sa_sigaction = SignalHandler;
		sigaction(SIGILL, &sigAction, NULL);
		sigaction(SIGSEGV, &sigAction, NULL);
		sigaction(SIGFPE, &sigAction, NULL);
		sigaction(SIGBUS, &sigAction, NULL);
		sigaction(SIGINT, &sigAction, NULL);
		sigaction(SIGQUIT, &sigAction, NULL);
		sigaction(SIGTERM, &sigAction, NULL);
		sigaction(SIGTRAP, &sigAction, NULL);

		g_platInitialized = true;
	}
}

BASEAPI void Plat_Shutdown()
{
	g_platInitialized = false;
}

s32 s_argc;
char** s_argv;

extern "C" BASEAPI void Base_Internal_SetArgs(s32 argc, char* argv[])
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

template <class T1, class T2, class T3, class T4, class T5, class T6>
ATTRIBUTE(naked)
uptr Base_SysCall(uptr number, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
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
		error = errno;
	}

	BREAKPOINT();

	exit(error);
	__builtin_unreachable();
}

bool Base_GetSystemMemory(ssize size)
{
	// Linked list nodes, can contain any size of allocation, but there's a limit to the number of OS allocations
	static IntrusiveLinkedNode<SystemAllocation> memoryNodes[64];

	// Maximize available space by rounding up to page size directly
	size = AlignUp(size, getpagesize());

	ASSERT_MSG(
		g_memInfo.allocations.Size() < ArraySize(memoryNodes),
		"OS allocation nodes exhausted, increase the size of the memory nodes array");

	IntrusiveLinkedNode<SystemAllocation>* node = &memoryNodes[g_memInfo.allocations.Size()];
	node->data.size = size;

	node->data.memory =
		reinterpret_cast<void*>(mmap(nullptr, node->data.size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
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
	static const char DLL_PREFIX[] = "lib";
	static const char DLL_EXT[] = ".so";

	CString fullName = name;
	ssize index = fullName.Find('/', true);
	fullName.Add(DLL_PREFIX, index == CString::BAD_INDEX ? 0 : index);
	if (fullName.Find(DLL_EXT) == CString::BAD_INDEX)
	{
		fullName.Add(DLL_EXT);
	}
	Log_Debug("Loading library %s", fullName.Data());

	void* base = dlopen(fullName.Data(), RTLD_GLOBAL | RTLD_LAZY);
	if (!base)
	{
		Log_Error("Failed to load library %s: %s", fullName.Data(), dlerror());
		return nullptr;
	}

	return new CUnixLibrary(name, base);
}

CUnixLibrary::CUnixLibrary(cstr name, void* base) : m_name(Base_StrClone(name)), m_base(base)
{
}

CUnixLibrary::~CUnixLibrary()
{
	if (!m_name)
	{
		Base_Free(m_name);
	}
}

void CUnixLibrary::Unload()
{
	if (m_base)
	{
		dlclose(m_base);
		m_base = nullptr;
	}
}

void* CUnixLibrary::GetSymbol(cstr name, bool optional)
{
	void* sym = dlsym(m_base, name);
	if (!sym)
	{
		Log_Message(optional ? LogLevel::Warning : LogLevel::Error, "Failed to get symbol %s: %d", name, errno);
		return nullptr;
	}

	return sym;
}

BASEAPI bool Plat_ConsoleHasColor()
{
	// safe enough to assume, identifying the terminal is probably terrible
	return true;
}

BASEAPI void Plat_WriteConsole(cstr message)
{
	write(STDOUT_FD, message, Base_StrLength(message));
}

BASEAPI cstr Plat_GetDataLocation()
{
	static char s_saveDir[1024];

	if (!Base_StrLength(s_saveDir))
	{
		if (Plat_GetEnvironment("XDG_USER_DATA_HOME") && Base_StrLength(Plat_GetEnvironment("XDG_USER_DATA_HOME")))
		{
			Base_StrFormat(s_saveDir, ArraySize(s_saveDir), "%s" GAME_NAME "/", Plat_GetEnvironment("XDG_USER_DATA_HOME"));
		}
		else if (Plat_GetEnvironment("HOME") && Base_StrLength(Plat_GetEnvironment("HOME")))
		{
			Base_StrFormat(s_saveDir, ArraySize(s_saveDir), "%s/.local/share/" GAME_NAME "/", Plat_GetEnvironment("HOME"));
		}
		else
		{
			// good enough
			Base_StrCopy(s_saveDir, "/tmp/" GAME_NAME "/", ArraySize(s_saveDir));
		}
	}

	return s_saveDir;
}

BASEAPI cstr Plat_GetEngineDir()
{
	static char s_directory[PATH_MAX + 1] = {0};

	if (!Base_StrLength(s_directory))
	{
		readlink("/proc/self/exe", s_directory, ArraySize<usize>(s_directory));
		ssize exeName = Base_StrFind(s_directory, '/', true); // chances that this fails are 0
		s_directory[exeName] = 0;
	}

	return s_directory;
}

BASEAPI cstr Plat_GetEnvironment(cstr name)
{
	return getenv(name);
}

BASEAPI u64 Plat_GetMilliseconds()
{
	struct timespec time = {};

	clock_gettime(CLOCK_REALTIME, &time);

	return time.tv_sec * 1000 + (time.tv_nsec + 500000) / 1000000;
}

/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400 * (31 + 29))

#define DAYS_PER_400Y (365 * 400 + 97)
#define DAYS_PER_100Y (365 * 100 + 24)
#define DAYS_PER_4Y   (365 * 4 + 1)

// https://git.musl-libc.org/cgit/musl/tree/src/time/__secs_to_tm.c
BASEAPI void Plat_GetDateTime(DateTime& time, bool utc)
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
