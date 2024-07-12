TITLE PowerPC Windows function stubs

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
ENDM

; xboxkrnl
MAKE_STUB DbgPrint
MAKE_STUB NtAllocateVirtualMemory
MAKE_STUB NtFreeVirtualMemory
MAKE_STUB RtlAnsiStringToUnicodeString
MAKE_STUB RtlFreeUnicodeString
MAKE_STUB XexGetProcedureAddress

END
