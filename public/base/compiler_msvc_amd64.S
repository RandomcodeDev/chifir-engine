# \file MSVC AMD64 freestanding support (GNU syntax)
# \copyright 2025 Randomcode Developers

.intel_syntax noprefix

.data

errorMsg: .asciz "Security cookie has wrong value"

.extern __security_cookie

.globl _fltused
_fltused: .long 0x9875

.text

.extern Base_AbortSafeC

.ifdef CH_STATIC
.extern __chkstk_Forwarder
.globl __chkstk
__chkstk:
	jmp __chkstk_Forwarder
.endif

.globl __security_check_cookie
__security_check_cookie:
	cmp rcx, [__security_cookie]
	jnz Fail

	# Instructions that would go here don't work with the security cookie value I choose to use

	ret

Fail:
	mov r8, rcx
	mov r9, __security_cookie
	mov ecx, 0xC0000409
	lea rdx, [errorMsg]
	call Base_AbortSafeC

.globl __GSHandlerCheck
__GSHandlerCheck:
	mov eax, 1
	ret


