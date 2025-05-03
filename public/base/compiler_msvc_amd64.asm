; \file MSVC AMD64 freestanding support
; \copyright 2025 Randomcode Developers

TITLE MSVC AMD64 support

OPTION PROLOGUE:NONE

.DATA

errorMsg db "Security cookie has wrong value"

EXTERN __security_cookie : QWORD

PUBLIC _fltused
_fltused DD 9875h

.CODE

EXTERN Base_AbortSafeC : PROC

IFDEF CH_STATIC
EXTERN __chkstk_Forwarder : PROC
PUBLIC __chkstk
__chkstk PROC
	jmp __chkstk_Forwarder
__chkstk ENDP
ENDIF

PUBLIC __security_check_cookie
__security_check_cookie PROC
	cmp rcx, [__security_cookie]
	jnz Fail

	; Instructions that would go here don't work with the security cookie value I choose to use

	ret

Fail:
	mov r8, rcx
	mov r9, __security_cookie
	mov ecx, 0C0000409h
	lea rdx, [errorMsg]
	call Base_AbortSafeC
__security_check_cookie ENDP

PUBLIC __GSHandlerCheck
__GSHandlerCheck PROC
	mov eax, 1
	ret
__GSHandlerCheck ENDP

END
