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
	int _fltused = 0x9875;

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

	// TODO: make sure this works on 32-bit and non-x86
	void __GSHandlerCheckCommon(void* EstablisherFrame, void* DispatcherContext, u32* GSHandlerData)

	{
#ifdef CH_AMD64
		uptr uVar1;
		void* pvVar2;

		/* WARNING: Load size is inaccurate */
		/* WARNING: Load size is inaccurate */
		pvVar2 = EstablisherFrame;
		if ((*GSHandlerData & 4) != 0)
		{
			pvVar2 = (void*)((sptr) * (int*)((sptr)GSHandlerData + 4) + (sptr)EstablisherFrame &
							 (sptr) - *(int*)((sptr)GSHandlerData + 8));
		}
		uVar1 = (uptr) * (unsigned int*)(*(sptr*)((sptr)DispatcherContext + 0x10) + 8);
		if ((*(unsigned char*)(uVar1 + 3 + *(sptr*)((sptr)DispatcherContext + 8)) & 0xf) != 0)
		{
			EstablisherFrame = (void*)((sptr)EstablisherFrame +
									   (uptr)(*(unsigned char*)(uVar1 + 3 + *(sptr*)((sptr)DispatcherContext + 8)) & 0xfffffff0));
		}
		__security_check_cookie((uptr)EstablisherFrame ^ *(uptr*)((sptr)(int)(*GSHandlerData & 0xfffffff8) + (sptr)pvVar2));
#else
		(void)EstablisherFrame;
		(void)DispatcherContext;
		(void)GSHandlerData;
#endif
	}

	u8 __GSHandlerCheck(
		unsigned char ExceptionRecord, void* EstablisherFrame, unsigned char ContextRecord, void* DispatcherContext)

	{
#ifdef CH_AMD64
		(void)ContextRecord;
		(void)ExceptionRecord;
		__GSHandlerCheckCommon(EstablisherFrame, DispatcherContext, *(u32**)((sptr)DispatcherContext + 0x38));
#else
		(void)ExceptionRecord;
		(void)EstablisherFrame;
		(void)ContextRecord;
		(void)DispatcherContext;
#endif
		return true;
	}

#if _MSC_VER >= 1700
	ATTRIBUTE(guard(suppress))
#endif
	void* __CxxFrameHandler3()
	{
		Base_QuitSafe(STATUS_UNHANDLED_EXCEPTION, "C++ exception 3");
		// return nullptr;
	}

#if _MSC_VER >= 1700
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

#ifndef __clang__
// Ensures the vtable for type_info is generated
type_info::~type_info()
{
}
#endif
