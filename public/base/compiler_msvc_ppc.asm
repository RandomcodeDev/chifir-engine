TITLE MSVC PowerPC support

OPTION PROLOGUE:NONE

.DATA
cookieMsg db "Security cookie has wrong value"

EXTERN __security_cookie : DWORD

.CODE

Base_QuitSafe TEXTEQU <?Base_QuitSafe@@YAXHPBD@Z>
EXTERN Base_QuitSafe : PROC

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
	lis r0, Base_QuitSafe
	addi r0, r0, Base_QuitSafe
	mtctr r0
	bctr
__security_check_cookie ENDP

END

