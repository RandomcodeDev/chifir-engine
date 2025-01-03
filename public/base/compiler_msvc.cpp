/// Implements/stubs everything MSVC asks for, like the security cookie, GS handling, RTC stuff
/// This is basically because it's too hard to separate the runtime from certain compiler builtins,
/// so this file implements the bare minimum, at times using functions from the engine to replace
/// functionality normally provided by the CRT.

#include "base.h"
#include "compiler.h"
#include "platform.h"
#include "types.h"

typedef void (*_PVFV)(void);
typedef int (*_PIFV)(void);
typedef void (*_PVFI)(int);

extern "C"
{
	/// C initializers
#pragma section(".CRT$XIA", long, read)
	__declspec(allocate(".CRT$XIA")) _PIFV __xi_a[] = {0};
#pragma section(".CRT$XIZ", long, read)
	__declspec(allocate(".CRT$XIZ")) _PIFV __xi_z[] = {0};

	/// C++ constructors
#pragma section(".CRT$XCA", long, read)
	__declspec(allocate(".CRT$XCA")) _PVFV __xc_a[] = {0};
#pragma section(".CRT$XCZ", long, read)
	__declspec(allocate(".CRT$XCZ")) _PVFV __xc_z[] = {0};

	/// C pre-terminators
#pragma section(".CRT$XPA", long, read)
	__declspec(allocate(".CRT$XPA")) _PVFV __xp_a[] = {0};
#pragma section(".CRT$XPZ", long, read)
	__declspec(allocate(".CRT$XPZ")) _PVFV __xp_z[] = {0};

	/// C terminators
#pragma section(".CRT$XTA", long, read)
	__declspec(allocate(".CRT$XTA")) _PVFV __xt_a[] = {0};
#pragma section(".CRT$XTZ", long, read)
	__declspec(allocate(".CRT$XTZ")) _PVFV __xt_z[] = {0};

#pragma comment(linker, "/merge:.CRT=.rdata")

#if defined CH_AMD64 || defined CH_ARM64
	ATTRIBUTE(align(64)) uptr __security_cookie = 0x6942069420694206;
#else
	ATTRIBUTE(align(64)) uptr __security_cookie = 0x69420694;
#endif
	uptr __security_cookie_complement;

	void __cdecl __security_init_cookie()
	{
		/// TODO: randomize the cookie here

		__security_cookie_complement = ~__security_cookie;
	}

	NORETURN void __cdecl __report_rangecheckfailure()
	{
		Base_AbortSafe(STATUS_STACK_BUFFER_OVERRUN, "Range check failure");
	}

	/// TODO: figure these out
	void __cdecl _RTC_InitBase()
	{
	}

	void __cdecl _RTC_ShutDown()
	{
	}

	void _RTC_CheckStackVars()
	{
	}

	void __cdecl _RTC_CheckEsp()
	{
	}

	void _RTC_Shutdown()
	{
	}

	void __chkstk(uptr addr)
	{
		(void)addr;
	}

#if !defined __clang__ && _MSC_VER >= 1700
	ATTRIBUTE(guard(suppress))
#endif
	void* __CxxFrameHandler3()
	{
		Base_AbortSafe(STATUS_UNHANDLED_EXCEPTION, "C++ exception 3");
		/// return nullptr;
	}

#if !defined __clang__ && _MSC_VER >= 1700
	ATTRIBUTE(guard(suppress))
#endif
	void* __CxxFrameHandler4()
	{
		Base_Abort(STATUS_UNHANDLED_EXCEPTION, "C++ exception 4");
		/// return nullptr;
	}

	int __cdecl _purecall()
	{
		/// As far as I can tell, this gets called when a virtual call has no implementation, and normally code to call a handler
		/// would be here. If I cared and this function wasn't implemented directly, the handler would have this same code, which
		/// means that functionality isn't needed here.
		Base_AbortSafe(STATUS_NOT_FOUND, "Pure virtual call");
	}

	/// TODO: unstub this so global destructors work
	int __cdecl atexit(void (*func)())
	{
		(void)func;
		return 0;
	}
}

static void CallXtors(_PVFV* begin, _PVFV* end)
{
	for (_PVFV* iter = begin; iter != end; iter++)
	{
		if (*iter)
		{
			(**iter)();
		}
	}
}

static s32 CallXtors(_PIFV* begin, _PIFV* end)
{
	s32 ret = 0;
	for (_PIFV* iter = begin; iter != end; iter++)
	{
		if (*iter)
		{
			ret = (**iter)();
			if (ret != 0)
			{
				return ret;
			}
		}
	}

	return 0;
}

void RunGlobalConstructors()
{
	s32 ret = CallXtors(__xi_a, __xi_z);
	if (ret != 0)
	{
		Base_AbortSafe(ret, "A global constructor failed");
	}
	CallXtors(__xc_a, __xc_z);
}

void RunGlobalDestructors()
{
	CallXtors(__xp_a, __xp_z);
	CallXtors(__xt_a, __xt_z);
}

/// Ensures the vtable for type_info is generated
type_info::~type_info()
{
}
