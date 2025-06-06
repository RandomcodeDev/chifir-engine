MAKE_STUB _chkstk,

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
MAKE_STUB NtCreateMutant, @16
MAKE_STUB NtCreateThread, @32
MAKE_STUB NtCreateThreadEx, @44
MAKE_STUB NtDelayExecution, @8
MAKE_STUB NtFreeVirtualMemory, @16
MAKE_STUB NtProtectVirtualMemory, @20
MAKE_STUB NtQueryFullAttributesFile, @8
MAKE_STUB NtQueryInformationFile, @20
MAKE_STUB NtQueryInformationThread, @20
MAKE_STUB NtQuerySystemInformation, @16
MAKE_STUB NtQuerySystemInformationEx, @24
MAKE_STUB NtQueryValueKey, @24
MAKE_STUB NtRaiseHardError, @24
MAKE_STUB NtReadFile, @36
MAKE_STUB NtReleaseMutant, @8
MAKE_STUB NtResumeThread, @8
MAKE_STUB NtTerminateProcess, @8
MAKE_STUB NtWaitForSingleObject, @12
MAKE_STUB NtWriteFile, @36
MAKE_STUB NtYieldExecution, @0

MAKE_STUB RtlAddVectoredExceptionHandler, @8
MAKE_STUB RtlAnsiStringToUnicodeString, @12
MAKE_STUB RtlCaptureStackBackTrace, @16
MAKE_STUB RtlFreeAnsiString, @4
MAKE_STUB RtlFreeHeap, @12
MAKE_STUB RtlFreeUnicodeString, @4
MAKE_STUB RtlGetFullPathName_U, @16
MAKE_STUB RtlInitializeContext, @20
MAKE_STUB RtlTimeToTimeFields, @8
MAKE_STUB RtlUnicodeStringToAnsiString, @12

MAKE_STUB AllocConsole, @0
MAKE_STUB AttachConsole, @4
MAKE_STUB GetConsoleMode, @8
MAKE_STUB GetStdHandle, @4
MAKE_STUB SetConsoleMode, @8
MAKE_STUB SetConsoleCtrlHandler, @8
MAKE_STUB TlsAlloc, @0
MAKE_STUB TlsGetValue, @4
MAKE_STUB TlsSetValue, @8
MAKE_STUB WriteConsoleA, @20

MAKE_STUB SHGetFolderPathA, @20

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
MAKE_STUB IsImmersiveProcess, @4
MAKE_STUB LoadCursorA, @8
MAKE_STUB PeekMessageA, @20
MAKE_STUB RegisterClassExA, @4
MAKE_STUB SetProcessDPIAware, @0
MAKE_STUB SetWindowLongA, @12
MAKE_STUB SetWindowTextA, @8
MAKE_STUB ShowWindow, @8
MAKE_STUB TranslateMessage, @4
MAKE_STUB UnregisterClassA, @8

MAKE_STUB CoTaskMemAlloc, @4
MAKE_STUB RoInitialize, @4
MAKE_STUB RoUninitialize, @0
MAKE_STUB RoGetActivationFactory, @12
MAKE_STUB WindowsCreateString, @12
MAKE_STUB WindowsDeleteString, @4
MAKE_STUB WindowsGetStringRawBuffer, @8
