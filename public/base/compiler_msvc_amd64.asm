TITLE MSVC AMD64 support

OPTION PROLOGUE:NONE

.DATA

errorMsg db "Security cookie has wrong value"

EXTERN __security_cookie : QWORD

PUBLIC _fltused
_fltused DD 9875h

.CODE

Base_AbortSafe TEXTEQU <?Base_AbortSafe@@YAXHPEBD@Z>
EXTERN Base_AbortSafe : PROC

Base_MemSet TEXTEQU <?Base_MemSet@@YAPEAXPEAXI_J@Z>
EXTERN Base_MemSet : PROC

Base_MemCopy TEXTEQU <?Base_MemCopy@@YAPEAXPEIAXPEIBX_J@Z>
EXTERN Base_MemCopy : PROC

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
	call Base_AbortSafe
__security_check_cookie ENDP

PUBLIC memset
memset PROC
	jmp Base_MemSet
memset ENDP

PUBLIC memcpy
memcpy PROC
	jmp Base_MemCopy
memcpy ENDP

PUBLIC __GSHandlerCheck
__GSHandlerCheck PROC
	mov eax, 1
	ret
__GSHandlerCheck ENDP

END

