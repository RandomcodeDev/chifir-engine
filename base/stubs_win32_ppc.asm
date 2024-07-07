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

MAKE_STUB DbgPrint
MAKE_STUB LdrAddRefDll
MAKE_STUB LdrGetProcedureAddress
MAKE_STUB LdrLoadDll
MAKE_STUB LdrUnloadDll
MAKE_STUB NtAllocateVirtualMemory
MAKE_STUB NtFreeVirtualMemory
MAKE_STUB NtQuerySystemInformation
MAKE_STUB NtRaiseHardError
MAKE_STUB NtTerminateProcess
MAKE_STUB RtlAnsiStringToUnicodeString
MAKE_STUB RtlFreeHeap
MAKE_STUB RtlFreeUnicodeString

END
