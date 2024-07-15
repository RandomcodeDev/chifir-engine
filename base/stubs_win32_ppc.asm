TITLE Xbox 360 function stubs

OPTION PROLOGUE:NONE

MAKE_STUB MACRO name
	.DATA
	.CODE
	; Return true, because it's defined in the kernel/XAM no matter what
	PUBLIC @CatStr(name, _Available)
	@CatStr(name, _Available) PROC
		li r0, 1
		blr
	@CatStr(name, _Available) ENDP
ENDM

; xboxkrnl
MAKE_STUB DbgPrint
MAKE_STUB NtAllocateVirtualMemory
MAKE_STUB NtFreeVirtualMemory
MAKE_STUB QueryPerformanceCounter

END
