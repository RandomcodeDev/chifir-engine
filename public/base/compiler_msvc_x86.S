# \file MSVC IA-32 freestanding support (GNU syntax)
# \copyright Randomcode Developers

.intel_syntax noprefix

.section .data
cookieMsg: .asciz "Security cookie has wrong value"

.extern ___security_cookie

.globl __fltused
__fltused: .long 0x9875

.section .code

.globl @_RTC_CheckStackVars@8
@_RTC_CheckStackVars@8:
    ret

.extern _Base_AbortSafeC

.ifdef CH_STATIC
.extern _chkstk_Forwarder
.globl __chkstk
__chkstk:
	jmp _chkstk_Forwarder
.endif

.globl @__security_check_cookie@4
@__security_check_cookie@4:
	cmp ecx, [___security_cookie]
	jnz Fail

	// Instructions that would go here don't work with the security cookie value I choose to use

	ret

Fail:
	lea ecx, [cookieMsg]
	push ecx
	push 0x0C0000409
	call _Base_AbortSafeC

//.globl _memset
//_memset:
//	jmp Base_MemSet

//.globl _memcpy
//_memcpy:
//	jmp Base_MemCopy

.globl __GSHandler0xCeck
__GSHandler0xCeck:
	mov eax, 1
	ret