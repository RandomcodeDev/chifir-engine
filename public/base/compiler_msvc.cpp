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

	// for assembly functions
	NORETURN void Base_AbortSafeC(s32 error, cstr msg)
	{
		Base_AbortSafe(error, msg);
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

#ifdef CH_IA32
// Float to long
void __declspec(naked) _ftol()
{
    /* *INDENT-OFF* */
    __asm {
        push        ebp
        mov         ebp,esp
        sub         esp,20h
        and         esp,0FFFFFFF0h
        fld         st(0)
        fst         dword ptr [esp+18h]
        fistp       qword ptr [esp+10h]
        fild        qword ptr [esp+10h]
        mov         edx,dword ptr [esp+18h]
        mov         eax,dword ptr [esp+10h]
        test        eax,eax
        je          integer_QnaN_or_zero
arg_is_not_integer_QnaN:
        fsubp       st(1),st
        test        edx,edx
        jns         positive
        fstp        dword ptr [esp]
        mov         ecx,dword ptr [esp]
        xor         ecx,80000000h
        add         ecx,7FFFFFFFh
        adc         eax,0
        mov         edx,dword ptr [esp+14h]
        adc         edx,0
        jmp         localexit
positive:
        fstp        dword ptr [esp]
        mov         ecx,dword ptr [esp]
        add         ecx,7FFFFFFFh
        sbb         eax,0
        mov         edx,dword ptr [esp+14h]
        sbb         edx,0
        jmp         localexit
integer_QnaN_or_zero:
        mov         edx,dword ptr [esp+14h]
        test        edx,7FFFFFFFh
        jne         arg_is_not_integer_QnaN
        fstp        dword ptr [esp+18h]
        fstp        dword ptr [esp+18h]
localexit:
        leave
        ret
    }
    /* *INDENT-ON* */
}

void _ftol2_sse()
{
    _ftol();
}

void _ftol2()
{
    _ftol();
}

// 64-bit math operators for 32-bit systems
void __declspec(naked) _allmul()
{
    /* *INDENT-OFF* */
    __asm {
        mov         eax, dword ptr[esp+8]
        mov         ecx, dword ptr[esp+10h]
        or          ecx, eax
        mov         ecx, dword ptr[esp+0Ch]
        jne         hard
        mov         eax, dword ptr[esp+4]
        mul         ecx
        ret         10h
hard:
        push        ebx
        mul         ecx
        mov         ebx, eax
        mov         eax, dword ptr[esp+8]
        mul         dword ptr[esp+14h]
        add         ebx, eax
        mov         eax, dword ptr[esp+8]
        mul         ecx
        add         edx, ebx
        pop         ebx
        ret         10h
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _alldiv()
{
    /* *INDENT-OFF* */
    __asm {
        push        edi
        push        esi
        push        ebx
        xor         edi,edi
        mov         eax,dword ptr [esp+14h]
        or          eax,eax
        jge         L1
        inc         edi
        mov         edx,dword ptr [esp+10h]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+14h],eax
        mov         dword ptr [esp+10h],edx
L1:
        mov         eax,dword ptr [esp+1Ch]
        or          eax,eax
        jge         L2
        inc         edi
        mov         edx,dword ptr [esp+18h]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+1Ch],eax
        mov         dword ptr [esp+18h],edx
L2:
        or          eax,eax
        jne         L3
        mov         ecx,dword ptr [esp+18h]
        mov         eax,dword ptr [esp+14h]
        xor         edx,edx
        div         ecx
        mov         ebx,eax
        mov         eax,dword ptr [esp+10h]
        div         ecx
        mov         edx,ebx
        jmp         L4
L3:
        mov         ebx,eax
        mov         ecx,dword ptr [esp+18h]
        mov         edx,dword ptr [esp+14h]
        mov         eax,dword ptr [esp+10h]
L5:
        shr         ebx,1
        rcr         ecx,1
        shr         edx,1
        rcr         eax,1
        or          ebx,ebx
        jne         L5
        div         ecx
        mov         esi,eax
        mul         dword ptr [esp+1Ch]
        mov         ecx,eax
        mov         eax,dword ptr [esp+18h]
        mul         esi
        add         edx,ecx
        jb          L6
        cmp         edx,dword ptr [esp+14h]
        ja          L6
        jb          L7
        cmp         eax,dword ptr [esp+10h]
        jbe         L7
L6:
        dec         esi
L7:
        xor         edx,edx
        mov         eax,esi
L4:
        dec         edi
        jne         L8
        neg         edx
        neg         eax
        sbb         edx,0
L8:
        pop         ebx
        pop         esi
        pop         edi
        ret         10h
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _aulldiv()
{
    /* *INDENT-OFF* */
    __asm {
        push        ebx
        push        esi
        mov         eax,dword ptr [esp+18h]
        or          eax,eax
        jne         L1
        mov         ecx,dword ptr [esp+14h]
        mov         eax,dword ptr [esp+10h]
        xor         edx,edx
        div         ecx
        mov         ebx,eax
        mov         eax,dword ptr [esp+0Ch]
        div         ecx
        mov         edx,ebx
        jmp         L2
L1:
        mov         ecx,eax
        mov         ebx,dword ptr [esp+14h]
        mov         edx,dword ptr [esp+10h]
        mov         eax,dword ptr [esp+0Ch]
L3:
        shr         ecx,1
        rcr         ebx,1
        shr         edx,1
        rcr         eax,1
        or          ecx,ecx
        jne         L3
        div         ebx
        mov         esi,eax
        mul         dword ptr [esp+18h]
        mov         ecx,eax
        mov         eax,dword ptr [esp+14h]
        mul         esi
        add         edx,ecx
        jb          L4
        cmp         edx,dword ptr [esp+10h]
        ja          L4
        jb          L5
        cmp         eax,dword ptr [esp+0Ch]
        jbe         L5
L4:
        dec         esi
L5:
        xor         edx,edx
        mov         eax,esi
L2:
        pop         esi
        pop         ebx
        ret         10h
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _allrem()
{
    /* *INDENT-OFF* */
    __asm {
        push        ebx
        push        edi
        xor         edi,edi
        mov         eax,dword ptr [esp+10h]
        or          eax,eax
        jge         L1
        inc         edi
        mov         edx,dword ptr [esp+0Ch]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+10h],eax
        mov         dword ptr [esp+0Ch],edx
L1:
        mov         eax,dword ptr [esp+18h]
        or          eax,eax
        jge         L2
        mov         edx,dword ptr [esp+14h]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+18h],eax
        mov         dword ptr [esp+14h],edx
L2:
        or          eax,eax
        jne         L3
        mov         ecx,dword ptr [esp+14h]
        mov         eax,dword ptr [esp+10h]
        xor         edx,edx
        div         ecx
        mov         eax,dword ptr [esp+0Ch]
        div         ecx
        mov         eax,edx
        xor         edx,edx
        dec         edi
        jns         L4
        jmp         L8
L3:
        mov         ebx,eax
        mov         ecx,dword ptr [esp+14h]
        mov         edx,dword ptr [esp+10h]
        mov         eax,dword ptr [esp+0Ch]
L5:
        shr         ebx,1
        rcr         ecx,1
        shr         edx,1
        rcr         eax,1
        or          ebx,ebx
        jne         L5
        div         ecx
        mov         ecx,eax
        mul         dword ptr [esp+18h]
        xchg        eax,ecx
        mul         dword ptr [esp+14h]
        add         edx,ecx
        jb          L6
        cmp         edx,dword ptr [esp+10h]
        ja          L6
        jb          L7
        cmp         eax,dword ptr [esp+0Ch]
        jbe         L7
L6:
        sub         eax,dword ptr [esp+14h]
        sbb         edx,dword ptr [esp+18h]
L7:
        sub         eax,dword ptr [esp+0Ch]
        sbb         edx,dword ptr [esp+10h]
        dec         edi
        jns         L8
L4:
        neg         edx
        neg         eax
        sbb         edx,0
L8:
        pop         edi
        pop         ebx
        ret         10h
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _aullrem()
{
    /* *INDENT-OFF* */
    __asm {
        push        ebx
        mov         eax,dword ptr [esp+14h]
        or          eax,eax
        jne         L1
        mov         ecx,dword ptr [esp+10h]
        mov         eax,dword ptr [esp+0Ch]
        xor         edx,edx
        div         ecx
        mov         eax,dword ptr [esp+8]
        div         ecx
        mov         eax,edx
        xor         edx,edx
        jmp         L2
L1:
        mov         ecx,eax
        mov         ebx,dword ptr [esp+10h]
        mov         edx,dword ptr [esp+0Ch]
        mov         eax,dword ptr [esp+8]
L3:
        shr         ecx,1
        rcr         ebx,1
        shr         edx,1
        rcr         eax,1
        or          ecx,ecx
        jne         L3
        div         ebx
        mov         ecx,eax
        mul         dword ptr [esp+14h]
        xchg        eax,ecx
        mul         dword ptr [esp+10h]
        add         edx,ecx
        jb          L4
        cmp         edx,dword ptr [esp+0Ch]
        ja          L4
        jb          L5
        cmp         eax,dword ptr [esp+8]
        jbe         L5
L4:
        sub         eax,dword ptr [esp+10h]
        sbb         edx,dword ptr [esp+14h]
L5:
        sub         eax,dword ptr [esp+8]
        sbb         edx,dword ptr [esp+0Ch]
        neg         edx
        neg         eax
        sbb         edx,0
L2:
        pop         ebx
        ret         10h
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _alldvrm()
{
    /* *INDENT-OFF* */
    __asm {
        push        edi
        push        esi
        push        ebp
        xor         edi,edi
        xor         ebp,ebp
        mov         eax,dword ptr [esp+14h]
        or          eax,eax
        jge         L1
        inc         edi
        inc         ebp
        mov         edx,dword ptr [esp+10h]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+14h],eax
        mov         dword ptr [esp+10h],edx
L1:
        mov         eax,dword ptr [esp+1Ch]
        or          eax,eax
        jge         L2
        inc         edi
        mov         edx,dword ptr [esp+18h]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+1Ch],eax
        mov         dword ptr [esp+18h],edx
L2:
        or          eax,eax
        jne         L3
        mov         ecx,dword ptr [esp+18h]
        mov         eax,dword ptr [esp+14h]
        xor         edx,edx
        div         ecx
        mov         ebx,eax
        mov         eax,dword ptr [esp+10h]
        div         ecx
        mov         esi,eax
        mov         eax,ebx
        mul         dword ptr [esp+18h]
        mov         ecx,eax
        mov         eax,esi
        mul         dword ptr [esp+18h]
        add         edx,ecx
        jmp         L4
L3:
        mov         ebx,eax
        mov         ecx,dword ptr [esp+18h]
        mov         edx,dword ptr [esp+14h]
        mov         eax,dword ptr [esp+10h]
L5:
        shr         ebx,1
        rcr         ecx,1
        shr         edx,1
        rcr         eax,1
        or          ebx,ebx
        jne         L5
        div         ecx
        mov         esi,eax
        mul         dword ptr [esp+1Ch]
        mov         ecx,eax
        mov         eax,dword ptr [esp+18h]
        mul         esi
        add         edx,ecx
        jb          L6
        cmp         edx,dword ptr [esp+14h]
        ja          L6
        jb          L7
        cmp         eax,dword ptr [esp+10h]
        jbe         L7
L6:
        dec         esi
        sub         eax,dword ptr [esp+18h]
        sbb         edx,dword ptr [esp+1Ch]
L7:
        xor         ebx,ebx
L4:
        sub         eax,dword ptr [esp+10h]
        sbb         edx,dword ptr [esp+14h]
        dec         ebp
        jns         L9
        neg         edx
        neg         eax
        sbb         edx,0
L9:
        mov         ecx,edx
        mov         edx,ebx
        mov         ebx,ecx
        mov         ecx,eax
        mov         eax,esi
        dec         edi
        jne         L8
        neg         edx
        neg         eax
        sbb         edx,0
L8:
        pop         ebp
        pop         esi
        pop         edi
        ret         10h
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _aulldvrm()
{
    /* *INDENT-OFF* */
    __asm {
        push        esi
        mov         eax,dword ptr [esp+14h]
        or          eax,eax
        jne         L1
        mov         ecx,dword ptr [esp+10h]
        mov         eax,dword ptr [esp+0Ch]
        xor         edx,edx
        div         ecx
        mov         ebx,eax
        mov         eax,dword ptr [esp+8]
        div         ecx
        mov         esi,eax
        mov         eax,ebx
        mul         dword ptr [esp+10h]
        mov         ecx,eax
        mov         eax,esi
        mul         dword ptr [esp+10h]
        add         edx,ecx
        jmp         L2
L1:
        mov         ecx,eax
        mov         ebx,dword ptr [esp+10h]
        mov         edx,dword ptr [esp+0Ch]
        mov         eax,dword ptr [esp+8]
L3:
        shr         ecx,1
        rcr         ebx,1
        shr         edx,1
        rcr         eax,1
        or          ecx,ecx
        jne         L3
        div         ebx
        mov         esi,eax
        mul         dword ptr [esp+14h]
        mov         ecx,eax
        mov         eax,dword ptr [esp+10h]
        mul         esi
        add         edx,ecx
        jb          L4
        cmp         edx,dword ptr [esp+0Ch]
        ja          L4
        jb          L5
        cmp         eax,dword ptr [esp+8]
        jbe         L5
L4:
        dec         esi
        sub         eax,dword ptr [esp+10h]
        sbb         edx,dword ptr [esp+14h]
L5:
        xor         ebx,ebx
L2:
        sub         eax,dword ptr [esp+8]
        sbb         edx,dword ptr [esp+0Ch]
        neg         edx
        neg         eax
        sbb         edx,0
        mov         ecx,edx
        mov         edx,ebx
        mov         ebx,ecx
        mov         ecx,eax
        mov         eax,esi
        pop         esi
        ret         10h
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _allshl()
{
    /* *INDENT-OFF* */
    __asm {
        cmp         cl,40h
        jae         RETZERO
        cmp         cl,20h
        jae         MORE32
        shld        edx,eax,cl
        shl         eax,cl
        ret
MORE32:
        mov         edx,eax
        xor         eax,eax
        and         cl,1Fh
        shl         edx,cl
        ret
RETZERO:
        xor         eax,eax
        xor         edx,edx
        ret
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _allshr()
{
    /* *INDENT-OFF* */
    __asm {
        cmp         cl,3Fh
        jae         RETSIGN
        cmp         cl,20h
        jae         MORE32
        shrd        eax,edx,cl
        sar         edx,cl
        ret
MORE32:
        mov         eax,edx
        sar         edx,1Fh
        and         cl,1Fh
        sar         eax,cl
        ret
RETSIGN:
        sar         edx,1Fh
        mov         eax,edx
        ret
    }
    /* *INDENT-ON* */
}

void __declspec(naked) _aullshr()
{
    /* *INDENT-OFF* */
    __asm {
        cmp         cl,40h
        jae         RETZERO
        cmp         cl,20h
        jae         MORE32
        shrd        eax,edx,cl
        shr         edx,cl
        ret
MORE32:
        mov         eax,edx
        xor         edx,edx
        and         cl,1Fh
        shr         eax,cl
        ret
RETZERO:
        xor         eax,eax
        xor         edx,edx
        ret
    }
    /* *INDENT-ON* */
}
#endif
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
