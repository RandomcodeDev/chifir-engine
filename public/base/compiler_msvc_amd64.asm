TITLE MSVC __security_check_cookie

OPTION PROLOGUE:NONE

.DATA

errorMsg db "Security cookie has wrong value"

EXTERN __security_cookie : QWORD

.CODE

Base_QuitSafe TEXTEQU <?Base_QuitSafe@@YAXHPEBD@Z>
EXTERN Base_QuitSafe : PROC

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
	call Base_QuitSafe
__security_check_cookie ENDP

END

