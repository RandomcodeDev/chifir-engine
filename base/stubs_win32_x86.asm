.386
.MODEL FLAT

; I get that code had to be small back then, but fuck stdcall
MAKE_STUB MACRO name, callingConv, paramSize
	@CatStr(extrnlFwd_, name) TEXTEQU @CatStr(name, _Forwarder)
	PUBLIC @CatStr(_STUB_, name)
	.DATA
	PUBLIC @CatStr(__imp__, name, @, paramSize)
	@CatStr(__imp__, name, @, paramSize) LABEL DWORD
	@CatStr(_STUB_, name) DD 0
	.CODE
	@CatStr(extrnlFwd_, name) PROC
		jmp [DWORD PTR @CatStr(_STUB_, name)]
	@CatStr(extrnlFwd_, name) ENDP
ENDM

MAKE_STUB DbgPrint, __stdcall, 4
MAKE_STUB LdrAddRefDll, __stdcall, 8
MAKE_STUB LdrGetProcedureAddress, __stdcall, 16
MAKE_STUB LdrLoadDll, __stdcall, 16
MAKE_STUB LdrUnloadDll, __stdcall, 4
MAKE_STUB NtAllocateVirtualMemory, __stdcall, 24
MAKE_STUB NtFreeVirtualMemory, __stdcall, 16
MAKE_STUB NtQuerySystemInformation, __stdcall, 16
MAKE_STUB NtRaiseHardError, __stdcall, 0
MAKE_STUB NtTerminateProcess, __stdcall, 8
MAKE_STUB RtlAnsiStringToUnicodeString, __stdcall, 12
MAKE_STUB RtlFreeHeap, __stdcall, 12
MAKE_STUB RtlFreeUnicodeString, __stdcall, 4

END
