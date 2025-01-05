TITLE AMD64 Windows function stubs

OPTION PROLOGUE:NONE

; I get that code had to be small back then, but fuck stdcall
MAKE_STUB MACRO name
	.DATA
	PUBLIC @CatStr(__imp_, name)
	@CatStr(__imp_, name) LABEL DWORD
	PUBLIC @CatStr(STUB_, name)
	@CatStr(STUB_, name) DQ 0
	.CODE
	@CatStr(name, _Forwarder) PROC
		jmp [QWORD PTR @CatStr(STUB_, name)]
	@CatStr(name, _Forwarder) ENDP
	PUBLIC @CatStr(name, _Available)
	@CatStr(name, _Available) PROC
		push rbp
		mov rbp, rsp
		mov rax, @CatStr(STUB_, name)
		cmp rax, 0
		jne Avail
		mov eax, 0
		pop rbp
		ret
Avail:
		mov eax, 1
		pop rbp
		ret
	@CatStr(name, _Available) ENDP
ENDM

; ntdll stubs
MAKE_STUB DbgPrint
MAKE_STUB LdrAddRefDll
MAKE_STUB LdrAddDllDirectory
MAKE_STUB LdrGetProcedureAddress
MAKE_STUB LdrLoadDll
MAKE_STUB LdrUnloadDll
MAKE_STUB NtAllocateVirtualMemory
MAKE_STUB NtClose
MAKE_STUB NtCreateFile
MAKE_STUB NtFreeVirtualMemory
MAKE_STUB NtQueryFullAttributesFile
MAKE_STUB NtQueryInformationFile
MAKE_STUB NtQuerySystemInformation
MAKE_STUB NtRaiseHardError
MAKE_STUB NtReadFile
MAKE_STUB NtTerminateProcess
MAKE_STUB NtWriteFile
MAKE_STUB RtlAnsiStringToUnicodeString
MAKE_STUB RtlFreeAnsiString
MAKE_STUB RtlFreeHeap
MAKE_STUB RtlFreeUnicodeString
MAKE_STUB RtlGetFullPathName_U
MAKE_STUB RtlTimeToTimeFields
MAKE_STUB RtlUnicodeStringToAnsiString

; kernel32 stubs
MAKE_STUB AllocConsole
MAKE_STUB AttachConsole
MAKE_STUB GetConsoleMode
MAKE_STUB GetStdHandle
MAKE_STUB SetConsoleMode

; shell32 stubs
MAKE_STUB SHGetFolderPathA

; user32 stubs
MAKE_STUB AdjustWindowRect
MAKE_STUB ClientToScreen
MAKE_STUB CreateWindowExA
MAKE_STUB DefWindowProcA
MAKE_STUB DestroyWindow
MAKE_STUB DispatchMessageA
MAKE_STUB GetClassInfoExA
MAKE_STUB GetClientRect
MAKE_STUB GetDpiForWindow
MAKE_STUB GetSystemMetrics
MAKE_STUB GetWindowLongPtrA
MAKE_STUB LoadCursorA
MAKE_STUB PeekMessageA
MAKE_STUB RegisterClassExA
MAKE_STUB SetProcessDPIAware
MAKE_STUB SetWindowLongPtrA
MAKE_STUB SetWindowTextA
MAKE_STUB ShowWindow
MAKE_STUB TranslateMessage
MAKE_STUB UnregisterClassA

END
