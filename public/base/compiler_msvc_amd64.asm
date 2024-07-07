TITLE MSVC __security_check_cookie

OPTION PROLOGUE:NONE

.DATA
errorMsg db "Invalid cookie 0x%llX"

EXTERN __security_cookie : QWORD

.CODE

EXTERN ?Base_Quit@@YAXHPEBDZZ : PROC

; Can't have prologue/epilogue, seems to corrupt something
PUBLIC __security_check_cookie
__security_check_cookie PROC
	cmp rcx, [__security_cookie]
	jnz Fail

	; Instructions that would go here don't work with the security cookie value I choose to use

	ret

Fail:
	mov r8, rcx
	mov ecx, 0C0000409h
	lea rdx, [errorMsg]
	call ?Base_Quit@@YAXHPEBDZZ
__security_check_cookie ENDP

END

