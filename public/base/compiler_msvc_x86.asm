; \file MSVC IA-32 freestanding support
; \copyright 2025 Randomcode Developers

.386
.MODEL FLAT

TITLE MSVC x86 support

OPTION PROLOGUE:NONE

.DATA
cookieMsg db "Security cookie has wrong value"

EXTERN ___security_cookie : DWORD

PUBLIC __fltused
__fltused DD 9875h

.CODE

PUBLIC @_RTC_CheckStackVars@8
@_RTC_CheckStackVars@8 PROC
@_RTC_CheckStackVars@8 ENDP

EXTERN _Base_AbortSafeC : PROC

IFDEF CH_STATIC
IFNDEF CH_XBOX
;EXTERN _chkstk_Forwarder : PROC
;PUBLIC __chkstk
;__chkstk PROC
;	jmp _chkstk_Forwarder
;__chkstk ENDP
ENDIF
ENDIF

PUBLIC @__security_check_cookie@4
@__security_check_cookie@4 PROC
	cmp ecx, [___security_cookie]
	jnz Fail

	; Instructions that would go here don't work with the security cookie value I choose to use

	ret

Fail:
	lea ecx, [cookieMsg]
	push ecx
	push 0C0000409h
	call _Base_AbortSafeC
@__security_check_cookie@4 ENDP

;PUBLIC _memset
;_memset PROC
;	jmp Base_MemSet
;_memset ENDP

;PUBLIC _memcpy
;_memcpy PROC
;	jmp Base_MemCopy
;_memcpy ENDP

PUBLIC __GSHandlerCheck
__GSHandlerCheck PROC
	mov eax, 1
	ret
__GSHandlerCheck ENDP

END
