MAKE_STUB MACRO name
	.DATA
	PUBLIC @CatStr(__imp_, name)
	@CatStr(__imp_, name) LABEL QWORD
	PUBLIC @CatStr(STUB_, name)
	@CatStr(STUB_, name) DQ 0
	.CODE
	@CatStr(name, _Forwarder)
		ldr x0, =@CatStr(STUB_, name)
		ldr x1, [x0]
		br x1

	@CatStr(name, _Available)
		stp x29, x30, [sp, #-16]
		mov x29, sp

		ldr x0, =@CatStr(STUB_, name)
		ldr x1, [x0]
		cbz x1, Avail

		mov x0, #0
		b Done

Avail
		ldr x0, #1

Done
		ldp x29, x30, [sp], #16
		ret
ENDM

; ntdll stubs
MAKE_STUB DbgPrint
MAKE_STUB LdrAddRefDll
MAKE_STUB LdrAddDllDirectory
MAKE_STUB LdrGetDllHandle
MAKE_STUB LdrGetProcedureAddress
MAKE_STUB LdrLoadDll
MAKE_STUB LdrUnloadDll
MAKE_STUB NtAllocateVirtualMemory
MAKE_STUB NtClose
MAKE_STUB NtCreateFile
MAKE_STUB NtCreateKey
MAKE_STUB NtFreeVirtualMemory
MAKE_STUB NtQueryFullAttributesFile
MAKE_STUB NtQueryInformationFile
MAKE_STUB NtQuerySystemInformation
MAKE_STUB NtQueryValueKey
MAKE_STUB NtRaiseHardError
MAKE_STUB NtReadFile
MAKE_STUB NtTerminateProcess
MAKE_STUB NtWriteFile
MAKE_STUB RtlAnsiStringToUnicodeString
MAKE_STUB RtlCaptureStackBackTrace
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
MAKE_STUB WriteConsoleA

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
