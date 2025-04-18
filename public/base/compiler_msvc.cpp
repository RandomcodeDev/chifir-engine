/// \file MSVC freestanding support
/// \copyright Randomcode Developers

/// Implements/stubs everything MSVC asks for, like the security cookie, GS handling, RTC stuff
/// This is basically because it's too hard to separate the runtime from certain compiler builtins,
/// so this file implements the bare minimum, at times using functions from the engine to replace
/// functionality normally provided by the CRT.

#include "async.h"
#include "base.h"
#include "compiler.h"
#include "platform.h"
#include "types.h"

#ifdef IN_BASE
#include "../../base/base.h"
#endif

typedef void (*_PVFV)(void);
typedef int (*_PIFV)(void);
typedef void (*_PVFI)(int);

extern "C"
{
#pragma comment(linker, "/merge:.CRT=.rdata")

	// C initializers
#pragma section(".CRT$XIA", long, read)
	ATTRIBUTE(allocate(".CRT$XIA")) _PIFV __xi_a[] = {0};
#pragma section(".CRT$XIZ", long, read)
	ATTRIBUTE(allocate(".CRT$XIZ")) _PIFV __xi_z[] = {0};

	// C++ constructors
#pragma section(".CRT$XCA", long, read)
	ATTRIBUTE(allocate(".CRT$XCA")) _PVFV __xc_a[] = {0};
#pragma section(".CRT$XCZ", long, read)
	ATTRIBUTE(allocate(".CRT$XCZ")) _PVFV __xc_z[] = {0};

	// C pre-terminators
#pragma section(".CRT$XPA", long, read)
	ATTRIBUTE(allocate(".CRT$XPA")) _PVFV __xp_a[] = {0};
#pragma section(".CRT$XPZ", long, read)
	ATTRIBUTE(allocate(".CRT$XPZ")) _PVFV __xp_z[] = {0};

	// C terminators
#pragma section(".CRT$XTA", long, read)
	ATTRIBUTE(allocate(".CRT$XTA")) _PVFV __xt_a[] = {0};
#pragma section(".CRT$XTZ", long, read)
	ATTRIBUTE(allocate(".CRT$XTZ")) _PVFV __xt_z[] = {0};

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

#ifdef __clang__
	struct _ThrowInfo;
#endif
	void __stdcall _CxxThrowException(void* object, _ThrowInfo* throwInfo)
	{
		Base_Abort(STATUS_UNHANDLED_EXCEPTION, "Exception from object 0x%016X with throw info 0x%016X", object, throwInfo);
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

//	s32 _tls_index = 0;
//
//#pragma data_seg(".tls")
//#ifndef CH_I386
//	ATTRIBUTE(allocate(".tls"))
//#endif
//	u8 _tls_start = 0;
//#pragma data_seg(".tls$ZZZ")
//#ifndef CH_I386
//	ATTRIBUTE(allocate(".tls$ZZZ"))
//#endif
//	u8 _tls_end = 0;
//#pragma data_seg()
//
//	// these are called by ntdll, and that triggers calling __xd_*
//#pragma section(".CRT$XLA", long, read)
//	ATTRIBUTE(allocate(".CRT$XLA")) PIMAGE_TLS_CALLBACK __xl_a = nullptr;
//#pragma section(".CRT$XLZ", long, read)
//	ATTRIBUTE(allocate(".CRT$XLZ")) PIMAGE_TLS_CALLBACK __xl_z = nullptr;
//
//	// these are made by the compiler for individual variables
//#pragma section(".CRT$XDA", long, read)
//	ATTRIBUTE(allocate(".CRT$XDA")) _PVFV __xd_a = nullptr;
//#pragma section(".CRT$XDZ", long, read)
//	ATTRIBUTE(allocate(".CRT$XDZ")) _PVFV __xd_z = nullptr;
//
//#pragma data_seg(".rdata$T")
//#ifdef CH_IA32
//	ATTRIBUTE(allocate(".rdata$T"))
//	extern const IMAGE_TLS_DIRECTORY _tls_used = {
//		(u32)(uptr)&_tls_start,   // start of tls data
//		(u32)(uptr)&_tls_end,     // end of tls data
//		(u32)(uptr)&_tls_index,   // address of tls_index
//		(u32)(uptr)(&__xl_a + 1), // pointer to call back array
//		(u32)0,                   // size of tls zero fill
//		(u32)0                    // characteristics
//	};
//#else
//	ATTRIBUTE(allocate(".rdata$T"))
//	extern const IMAGE_TLS_DIRECTORY64 _tls_used = {
//		(u64)&_tls_start,   // start of tls data
//		(u64)&_tls_end,     // end of tls data
//		(u64)&_tls_index,   // address of tls_index
//		(u64)(&__xl_a + 1), // pointer to call back array
//		(u32)0,             // size of tls zero fill
//		{0}                 // characteristics
//	};
//#endif
//#pragma data_seg()
//
//#if defined(_MSC_VER) && !defined(__clang__)
//	[[msvc::no_tls_guard]]
//#endif
//	ATTRIBUTE(thread) bool __tls_guard = false;
//
//	void __stdcall __dyn_tls_init(void*, DWORD reason, void*)
//	{
//		if (reason == DLL_THREAD_ATTACH && !__tls_guard)
//		{
//			__tls_guard = true;
//			RunThreadConstructors();
//		}
//	}
//
//	// the compiler generates calls to this
//	void __cdecl __dyn_tls_on_demand_init() noexcept
//	{
//		__dyn_tls_init(nullptr, DLL_THREAD_ATTACH, nullptr);
//	}
//
//	unsigned long __cdecl __safe_get_tls_index() noexcept
//	{
//		__dyn_tls_on_demand_init();
//
//		return _tls_index;
//	}
//
//#pragma section(".CRT$XLC")
//	ATTRIBUTE(allocate(".CRT$XLC"))
//	PIMAGE_TLS_CALLBACK __xl_c = __dyn_tls_init; // tell ntdll to call this

	ATTRIBUTE(thread) s32 _Init_thread_epoch = INT32_MIN;

	// TODO: implement these properly once threads are implemented
	// they have to be aliased cause the compiler complains otherwise

	void __cdecl _Init_thread_header_impl(const s32* once) noexcept
	{
		(void)once;
	}
	ALIAS(_Init_thread_header_impl, _Init_thread_header)

	void __cdecl _Init_thread_footer_impl(const s32* once) noexcept
	{
		(void)once;
	}
	ALIAS(_Init_thread_footer_impl, _Init_thread_footer)
}

static void CallXtors(_PVFV* begin, _PVFV* end)
{
	for (_PVFV* iter = begin; iter != end; iter++)
	{
		if (*iter)
		{
			(*iter)();
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
			ret = (*iter)();
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
		NtTerminateProcess(NtCurrentProcess(), STATUS_FATAL_APP_EXIT);
		// Base_AbortSafe(ret, "A global constructor failed");
	}
	CallXtors(__xc_a, __xc_z);
}

void RunGlobalDestructors()
{
	CallXtors(__xp_a, __xp_z);
	CallXtors(__xt_a, __xt_z);
}

void __stdcall RunThreadConstructors()
{
//	CallXtors(&__xd_a + 1, &__xd_z);
}

/// Ensures the vtable for type_info is generated
// type_info::~type_info() noexcept
//{
// }
