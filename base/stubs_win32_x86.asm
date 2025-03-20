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
		push ebp
		mov ebp, esp
		mov eax, @CatStr(_STUB_, name)
		cmp eax, 0
		jne Avail
		mov eax, 0
		pop ebp
		ret
Avail:
		mov eax, 1
		pop ebp
		ret
	@CatStr(_, name, _Available) ENDP
ENDM

; ntdll stubs
MAKE_STUB __chkstk,
MAKE_STUB DbgPrint,
MAKE_STUB LdrAddRefDll, @8
MAKE_STUB LdrAddDllDirectory, @8
MAKE_STUB LdrGetDllHandle, @16
MAKE_STUB LdrGetProcedureAddress, @16
MAKE_STUB LdrLoadDll, @16
MAKE_STUB LdrUnloadDll, @4
MAKE_STUB NtAllocateVirtualMemory, @24
MAKE_STUB NtClose, @4
MAKE_STUB NtCreateFile, @44
MAKE_STUB NtCreateKey, @28
MAKE_STUB NtFreeVirtualMemory, @16
MAKE_STUB NtQueryFullAttributesFile, @8
MAKE_STUB NtQueryInformationFile, @20
MAKE_STUB NtQuerySystemInformation, @16
MAKE_STUB NtQueryValueKey, @24
MAKE_STUB NtRaiseHardError, @24
MAKE_STUB NtReadFile, @36
MAKE_STUB NtTerminateProcess, @8
MAKE_STUB NtWriteFile, @36
MAKE_STUB RtlAnsiStringToUnicodeString, @12
MAKE_STUB RtlCaptureStackBackTrace, @16
MAKE_STUB RtlFreeAnsiString, @4
MAKE_STUB RtlFreeHeap, @12
MAKE_STUB RtlFreeUnicodeString, @4
MAKE_STUB RtlGetFullPathName_U, @16
MAKE_STUB RtlTimeToTimeFields, @8
MAKE_STUB RtlUnicodeStringToAnsiString, @12

; kernel32 stubs
MAKE_STUB AllocConsole, @0
MAKE_STUB AttachConsole, @4
MAKE_STUB GetConsoleMode, @8
MAKE_STUB GetStdHandle, @4
MAKE_STUB SetConsoleMode, @8
MAKE_STUB WriteConsoleA, @20

; shell32 stubs
MAKE_STUB SHGetFolderPathA, @20

; user32 stubs
MAKE_STUB AdjustWindowRect, @12
MAKE_STUB ClientToScreen, @8
MAKE_STUB CreateWindowExA, @48
MAKE_STUB DefWindowProcA, @16
MAKE_STUB DestroyWindow, @4
MAKE_STUB DispatchMessageA, @4
MAKE_STUB GetClassInfoExA, @12
MAKE_STUB GetClientRect, @8
MAKE_STUB GetDpiForWindow, @4
MAKE_STUB GetSystemMetrics, @4
MAKE_STUB GetWindowLongA, @8
MAKE_STUB LoadCursorA, @8
MAKE_STUB PeekMessageA, @20
MAKE_STUB RegisterClassExA, @4
MAKE_STUB SetProcessDPIAware, @0
MAKE_STUB SetWindowLongA, @12
MAKE_STUB SetWindowTextA, @8
MAKE_STUB ShowWindow, @8
MAKE_STUB TranslateMessage, @4
MAKE_STUB UnregisterClassA, @8

END
