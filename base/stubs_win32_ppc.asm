TITLE PowerPC Windows function stubs

OPTION PROLOGUE:NONE

MAKE_STUB MACRO name
	.DATA
	PUBLIC name
	PUBLIC @CatStr(STUB_, name)
	name LABEL WORD
	@CatStr(STUB_, name) DW 0
	.CODE
	@CatStr(name, _Forwarder) PROC
		lis r0, name
		addi r0, r0, name
		lwz r0, 0(r0)
		mtctr r0
		bctr
	@CatStr(name, _Forwarder) ENDP
	PUBLIC @CatStr(_, name, _Available)
	@CatStr(_, name, _Available) PROC
		lis r0, name
		addi r0, r0, name
		lwz r0, 0(r0)
		cmplwi r0, 0
		bne Avail
		sti r0, 0
	Avail:
		sti r0, 1
		blr
	@CatStr(_, name, _Available) ENDP
ENDM

; xboxkrnl
MAKE_STUB DbgPrint
MAKE_STUB NtAllocateVirtualMemory
MAKE_STUB NtFreeVirtualMemory
MAKE_STUB RtlAnsiStringToUnicodeString
MAKE_STUB RtlFreeUnicodeString
MAKE_STUB XexGetProcedureAddress

END
