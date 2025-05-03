; \file MSVC Xbox 360 freestanding support
; \copyright 2025 Randomcode Developers

TITLE MSVC PowerPC support

OPTION PROLOGUE:NONE

.DATA
cookieMsg db "Security cookie has wrong value"

EXTERN __security_cookie : DWORD

PUBLIC _fltused
_fltused DD 9875h

.CODE

Base_AbortSafe TEXTEQU <?Base_AbortSafe@@YAXHPBD@Z>
EXTERN Base_AbortSafe : PROC

PUBLIC __security_check_cookie
__security_check_cookie PROC
	lis r0, __security_cookie
	addi r0, r0, __security_cookie
	cmpw r3, r0
	bne Fail
	blr

Fail:
	li r3, 0C0000409h
	lis r4, cookieMsg
	addi r4, r4, cookieMsg
	lis r0, Base_AbortSafe
	addi r0, r0, Base_AbortSafe
	mtctr r0
	bctr
__security_check_cookie ENDP

END
