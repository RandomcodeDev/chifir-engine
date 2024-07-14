.386
.MODEL FLAT

TITLE x86 Windows function stubs

OPTION PROLOGUE:NONE

; I get that code had to be small back then, but fuck stdcall
MAKE_STUB MACRO name, paramSize
	.DATA
	PUBLIC @CatStr(__imp__, name, paramSize)
	@CatStr(__imp__, name, paramSize) LABEL DWORD
	PUBLIC @CatStr(_STUB_, name)
	@CatStr(_STUB_, name) DD 0
	.CODE
	@CatStr(name, _Forwarder) PROC
		jmp [DWORD PTR @CatStr(_STUB_, name)]
	@CatStr(name, _Forwarder) ENDP
	PUBLIC @CatStr(_, name, _Available)
	@CatStr(_, name, _Available) PROC
		mov eax, @CatStr(_STUB_, name)
		ret
	@CatStr(_, name, _Available) ENDP
ENDM

; ntdll stubs
MAKE_STUB DbgPrint, 
MAKE_STUB LdrAddRefDll, @8
MAKE_STUB LdrAddDllDirectory, @8
MAKE_STUB LdrGetProcedureAddress, @16
MAKE_STUB LdrLoadDll, @16
MAKE_STUB LdrUnloadDll, @4
MAKE_STUB NtAllocateVirtualMemory, @24
MAKE_STUB NtFreeVirtualMemory, @16
MAKE_STUB NtQuerySystemInformation, @16
MAKE_STUB NtRaiseHardError, @0
MAKE_STUB NtTerminateProcess, @8
MAKE_STUB RtlAnsiStringToUnicodeString, @12
MAKE_STUB RtlFreeHeap, @12
MAKE_STUB RtlFreeUnicodeString, @4

; user32 stubs
MAKE_STUB AdjustWindowRect, @12
MAKE_STUB ClientToScreen, @8
MAKE_STUB CreateWindowExA, @48
MAKE_STUB DefWindowProcA, @16
MAKE_STUB DestroyWindow, @4
MAKE_STUB DispatchMessageA, @4
MAKE_STUB GetClientRect, @8
MAKE_STUB GetDpiForWindow, @4
MAKE_STUB GetSystemMetrics, @4
MAKE_STUB GetWindowLongA, @8
MAKE_STUB LoadCursorA, @8
MAKE_STUB PeekMessageA, @20
MAKE_STUB RegisterClassExA, @4
MAKE_STUB SetWindowLongA, @12
MAKE_STUB SetWindowTextA, @8
MAKE_STUB ShowWindow, @8
MAKE_STUB TranslateMessage, @4
MAKE_STUB UnregisterClassA, @8

END
