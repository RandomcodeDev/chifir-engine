// Implements/stubs everything MSVC asks for, like the security cookie, GS handling, RTC stuff
// This is basically because it's too hard to separate the runtime from certain compiler builtins,
// so this file implements the bare minimum, at times using functions from the engine to replace
// functionality normally provided by the CRT.

#include "base.h"
#include "compiler.h"
#include "platform.h"
#include "types.h"

extern "C"
{
#if defined CH_AMD64 || defined CH_ARM64
	ATTRIBUTE(align(64)) uptr __security_cookie = 0x6942069420694206;
#else
	ATTRIBUTE(align(64)) uptr __security_cookie = 0x69420694;
#endif
	uptr __security_cookie_complement;

	void __cdecl __security_init_cookie()
	{
		// TODO: randomize the cookie here

		__security_cookie_complement = ~__security_cookie;
	}

	NORETURN void __cdecl __report_rangecheckfailure()
	{
		Base_QuitSafe(STATUS_STACK_BUFFER_OVERRUN, "Range check failure");
	}

	// TODO: figure these out
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
		Base_QuitSafe(STATUS_UNHANDLED_EXCEPTION, "C++ exception 3");
		// return nullptr;
	}

#if !defined __clang__ && _MSC_VER >= 1700
	ATTRIBUTE(guard(suppress))
#endif
	void* __CxxFrameHandler4()
	{
		Base_Quit(STATUS_UNHANDLED_EXCEPTION, "C++ exception 4");
		// return nullptr;
	}

	int __cdecl _purecall()
	{
		// As far as I can tell, this gets called when a virtual call has no implementation, and normally code to call a handler
		// would be here. If I cared and this function wasn't implemented directly, the handler would have this same code, which
		// means that functionality isn't needed here.
		Base_QuitSafe(STATUS_NOT_FOUND, "Pure virtual call");
	}

	// TODO: unstub this so global destructors work
	int __cdecl atexit(void (*func)())
	{
		(void)func;
		return 0;
	}
}

// Ensures the vtable for type_info is generated
type_info::~type_info()
{
}
