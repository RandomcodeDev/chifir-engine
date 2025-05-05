/// \file Windows loader implementation
/// \copyright 2025 Randomcode Developers

/// This file is the reason Base.dll doesn't have an import table. It locates the base address of ntdll.dll using the
/// PEB_LDR_DATA structure, then it parses it and finds the address of LdrGetProcedureAddress by searching the export
/// table for a name that matches its hash. Then, it uses LdrGetProcedureAddress to fill in other stubs of system
/// functions, which are function pointers that use names the linker expects from an import library, allowing phnt's
/// correct declarations to be used everywhere without issue.
///
/// These are basically the same as the import table, but I get to do my own logic for filling them in, so that if some
/// function is missing from one version and not another, it isn't a fatal startup error that can't be avoided, allowing
/// for better portability. There are also forwarder functions that are re-exported under the original names so anything
/// else can use them. In other words, this file just does ntdll's job early in runtime instead of before the engine
/// runs.
///
/// On Xbox 360, things are different. xboxkrnl.exe and xam.xex export all the important functions exclusively by
/// ordinal, and are in the XEX format. The reason them only using ordinals isn't an issue is because they don't change
/// between kernel versions, and it's no longer updated, so that can't change. This means that the logic of this file
/// mostly doesn't apply to the 360. Because I don't know how the kernel does things, I've decided that just linking to
/// it is a much better idea.

#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/loader.h"
#include "base/log.h"
#include "platform_win32.h"

static PIMAGE_DOS_HEADER s_ntDllBase;

bool g_loaderInitialized;

// stubs are defined in stubs_win32_<arch>.asm

// These are in both Xbox 360 and desktop/OneCore

// ntdll/xboxkrnl
MAKE_STUB(DbgPrint, __cdecl)
MAKE_STUB(NtAllocateVirtualMemory, __stdcall, @24)
MAKE_STUB(NtFreeVirtualMemory, __stdcall, @16)
MAKE_STUB(RtlTimeToTimeFields, __stdcall, @8)

#ifndef CH_XENON
// These are desktop/OneCore things

// ntdll
#ifdef CH_IA32
MAKE_STUB(_chkstk, __cdecl)
#else
MAKE_STUB(__chkstk, __cdecl)
#endif
MAKE_STUB(LdrAddDllDirectory, __stdcall, @8)
MAKE_STUB(LdrAddRefDll, __stdcall, @8)
MAKE_STUB(LdrGetDllHandle, __stdcall, @16)
MAKE_STUB(LdrGetProcedureAddress, __stdcall, @16)
MAKE_STUB(LdrLoadDll, __stdcall, @16)
MAKE_STUB(LdrUnloadDll, __stdcall, @4)
MAKE_STUB(NtClose, __stdcall, @4)
MAKE_STUB(NtCreateFile, __stdcall, @44)
MAKE_STUB(NtCreateMutant, __stdcall, @16)
MAKE_STUB(NtCreateKey, __stdcall, @28)
MAKE_STUB(NtCreateThread, __stdcall, @32)
MAKE_STUB(NtCreateThreadEx, __stdcall, @44)
MAKE_STUB(NtDelayExecution, __stdcall, @8)
MAKE_STUB(NtFreeVirtualMemory, __stdcall, @16)
MAKE_STUB(NtProtectVirtualMemory, __stdcall, @20)
MAKE_STUB(NtQueryInformationFile, __stdcall, @20)
MAKE_STUB(NtQueryInformationThread, __stdcall, @20)
MAKE_STUB(NtQuerySystemInformation, __stdcall, @16)
MAKE_STUB(NtQuerySystemInformationEx, __stdcall, @24)
MAKE_STUB(NtQueryValueKey, __stdcall, @24)
MAKE_STUB(NtRaiseHardError, __stdcall, @24)
MAKE_STUB(NtReadFile, __stdcall, @36)
MAKE_STUB(NtReleaseMutant, __stdcall, @8)
MAKE_STUB(NtResumeThread, __stdcall, @8)
MAKE_STUB(NtTerminateProcess, __stdcall, @8)
MAKE_STUB(NtWaitForSingleObject, __stdcall, @12)
MAKE_STUB(NtWriteFile, __stdcall, @36)
MAKE_STUB(NtYieldExecution, __stdcall, @0)
MAKE_STUB(RtlAddVectoredExceptionHandler, __stdcall, @8)
MAKE_STUB(RtlAnsiStringToUnicodeString, __stdcall, @12)
MAKE_STUB(RtlCaptureStackBackTrace, __stdcall, @16)
MAKE_STUB(RtlFreeAnsiString, __stdcall, @4)
MAKE_STUB(RtlFreeHeap, __stdcall, @12)
MAKE_STUB(RtlFreeUnicodeString, __stdcall, @4)
MAKE_STUB(RtlGetFullPathName_U, __stdcall, @16)
MAKE_STUB(RtlInitializeContext, __stdcall, @20)
MAKE_STUB(RtlUnicodeStringToAnsiString, __stdcall, @12)

// kernel32
// Console stuff is all CSR calls I shouldn't rewrite
MAKE_STUB(AllocConsole, __stdcall, @0)
MAKE_STUB(AttachConsole, __stdcall, @4)
MAKE_STUB(GetConsoleMode, __stdcall, @8)
MAKE_STUB(GetStdHandle, __stdcall, @4)
MAKE_STUB(SetConsoleMode, __stdcall, @8)
MAKE_STUB(WriteConsoleA, __stdcall, @20)
// pre-Vista TLS was all in kernel32
MAKE_STUB(TlsAlloc, __stdcall, @0)
MAKE_STUB(TlsGetValue, __stdcall, @4)
MAKE_STUB(TlsSetValue, __stdcall, @8)

// shell32
// Shell stuff is evil
MAKE_STUB(SHGetFolderPathA, __stdcall, @20)

// user32
// user32 does some black magic in its startup that win32u/win32k require for the syscalls to work
// On modern Xboxes, GameOS has user32, while SystemOS only has UWP and win32kmin.sys
MAKE_STUB(AdjustWindowRect, __stdcall, @12)
MAKE_STUB(ClientToScreen, __stdcall, @8)
MAKE_STUB(CreateWindowExA, __stdcall, @48)
MAKE_STUB(DefWindowProcA, __stdcall, @16)
MAKE_STUB(DestroyWindow, __stdcall, @4)
MAKE_STUB(DispatchMessageA, __stdcall, @4)
MAKE_STUB(GetClassInfoExA, __stdcall, @12)
MAKE_STUB(GetClientRect, __stdcall, @8)
MAKE_STUB(GetDpiForWindow, __stdcall, @4)
MAKE_STUB(GetSystemMetrics, __stdcall, @4)
MAKE_STUB(GetWindowLongPtrA, __stdcall, @8)
MAKE_STUB(IsImmersiveProcess, __stdcall, @4)
MAKE_STUB(LoadCursorA, __stdcall, @8)
MAKE_STUB(PeekMessageA, __stdcall, @20)
MAKE_STUB(RegisterClassExA, __stdcall, @4)
MAKE_STUB(SetProcessDPIAware, __stdcall, @0)
MAKE_STUB(SetWindowLongPtrA, __stdcall, @12)
MAKE_STUB(SetWindowTextA, __stdcall, @8)
MAKE_STUB(ShowWindow, __stdcall, @8)
MAKE_STUB(TranslateMessage, __stdcall, @4)
MAKE_STUB(UnregisterClassA, __stdcall, @8)

// combase
MAKE_STUB(CoTaskMemAlloc, __stdcall, @4)
MAKE_STUB(RoInitialize, __stdcall, @4)
MAKE_STUB(RoUninitialize, __stdcall, @0)
MAKE_STUB(RoGetActivationFactory, __stdcall, @12)
MAKE_STUB(WindowsCreateString, __stdcall, @12)
MAKE_STUB(WindowsDeleteString, __stdcall, @4)
MAKE_STUB(WindowsGetStringRawBuffer, __stdcall, @8)
#endif

#ifndef CH_XENON
static bool FindNtDll(PPEB_LDR_DATA ldrData)
{
	// On desktop NTDLL is always the first image initialized, no matter what (there is no logical reason this would change)
	// On WoW64, this _does_ get the 32-bit NTDLL, which is the right one for this
	PLIST_ENTRY ntDllLink = ldrData->InInitializationOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY entry = CONTAINING_STRUCTURE(LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks, ntDllLink);
	s_ntDllBase = static_cast<PIMAGE_DOS_HEADER>(entry->DllBase);

	return s_ntDllBase != nullptr;
}

static u16 GetArchitecture()
{
#ifdef CH_XBOX
	return IMAGE_FILE_MACHINE_I386;
#else
	switch (USER_SHARED_DATA->NativeProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
		return IMAGE_FILE_MACHINE_AMD64;
	case PROCESSOR_ARCHITECTURE_ARM:
		return IMAGE_FILE_MACHINE_ARM;
	case PROCESSOR_ARCHITECTURE_ARM64:
		return IMAGE_FILE_MACHINE_ARM64;
	case PROCESSOR_ARCHITECTURE_IA64:
		return IMAGE_FILE_MACHINE_IA64;
	case PROCESSOR_ARCHITECTURE_INTEL:
		return IMAGE_FILE_MACHINE_I386;
	default:
		break;
	}
	return 0;
#endif
}

bool Base_CheckWoW64()
{
#ifdef CH_XBOX
	return false;
#else
	return GetArchitecture() != ((PIMAGE_NT_HEADERS)RVA_TO_VA(&__ImageBase, __ImageBase.e_lfanew))->FileHeader.Machine;
#endif
}

cstr Base_GetWineVersion()
{
	static bool checked = false;
	static bool wine = false;
	static char wineVersion[64];

	if (!checked)
	{
		CWindowsLibrary ntDllWin("ntdll.dll", s_ntDllBase);
		ILibrary* ntDll = &ntDllWin;
		auto f_wine_get_build_id = ntDll->GetSymbol<cstr (*)()>("wine_get_build_id");
		auto f_wine_get_host_version = ntDll->GetSymbol<void (*)(cstr*, cstr*)>("wine_get_host_version");

		wine = f_wine_get_build_id != nullptr;
		if (!wine)
		{
			return nullptr;
		}

		cstr host = nullptr;
		cstr hostVersion = nullptr;
		f_wine_get_host_version(&host, &hostVersion);
		Base_StrFormat(wineVersion, ArraySize(wineVersion), "%s on %s %s", f_wine_get_build_id(), host, hostVersion);

		checked = true;
	}

	if (wine)
	{
		return wineVersion;
	}

	return nullptr;
}

static bool FindLdrGetProcedureAddress()
{
	PIMAGE_NT_HEADERS ntHdrs = (PIMAGE_NT_HEADERS)RVA_TO_VA(s_ntDllBase, s_ntDllBase->e_lfanew);
	ASSERT(ntHdrs->Signature == IMAGE_NT_SIGNATURE); // Even if this isn't NTDLL, it sure as hell should have the right signature
	ASSERT(ntHdrs->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC);
	ASSERT_MSG(
		static_cast<ssize>(ntHdrs->OptionalHeader.SizeOfHeaders) >=
			(s_ntDllBase->e_lfanew + SIZEOF(IMAGE_NT_SIGNATURE) + SIZEOF(IMAGE_FILE_HEADER) + SIZEOF(IMAGE_OPTIONAL_HEADER) +
			 IMAGE_DIRECTORY_ENTRY_EXPORT * SIZEOF(IMAGE_DATA_DIRECTORY)),
		"Export directory not present");

	PIMAGE_DATA_DIRECTORY exportDirectory = &ntHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)RVA_TO_VA(s_ntDllBase, exportDirectory->VirtualAddress);

	u64 nameCount = exports->NumberOfNames;
	u32* nameAddrs = (u32*)RVA_TO_VA(s_ntDllBase, exports->AddressOfNames);
	u16* ordinals = (u16*)RVA_TO_VA(s_ntDllBase, exports->AddressOfNameOrdinals);
	u32* functions = (u32*)RVA_TO_VA(s_ntDllBase, exports->AddressOfFunctions);

	static const u64 TARGET_HASH = 0x49d2dcb66c6c2384;
	for (u64 i = 0; i < nameCount; i++)
	{
		cstr name = (cstr)RVA_TO_VA(s_ntDllBase, nameAddrs[i]);
		u64 hash = Base_Fnv1a64(name, Base_StrLength(name));
		if (hash == TARGET_HASH) // To get around collisions
		{
			STUB_NAME(LdrGetProcedureAddress) = (uptr (*)(...))RVA_TO_VA(s_ntDllBase, functions[ordinals[i]]);
			break;
		}
	}

	return LdrGetProcedureAddress_Available();
}
#endif

#define GET_FUNCTION_OPTIONAL(lib, name) STUB_NAME(name) = static_cast<ILibrary*>(lib)->GetSymbol<uptr (*)(...)>(STRINGIZE(name));
#define GET_FUNCTION(lib, name)                                                                                                  \
	{                                                                                                                            \
		GET_FUNCTION_OPTIONAL(lib, name)                                                                                         \
		if (!STUB_AVAILABLE(name)())                                                                                             \
		{                                                                                                                        \
			Base_Abort(LastNtStatus(), "Failed to get " STRINGIZE(name) " from " #lib ": NTSTATUS 0x%08X", LastNtStatus());      \
		}                                                                                                                        \
	}

#define GET_TEB32(teb) reinterpret_cast<PTEB32>(reinterpret_cast<uptr>(teb) + 0x2000)
#define GET_PEB32(peb) reinterpret_cast<PPEB32>(reinterpret_cast<uptr>(peb) - 0x1000)

bool Base_InitLoader()
{
	if (g_loaderInitialized)
	{
		return true;
	}

#ifndef CH_XENON
	if (!FindNtDll(NtCurrentPeb()->Ldr))
	{
		return false;
	}

	if (!FindLdrGetProcedureAddress())
	{
		return false;
	}

	CWindowsLibrary ntDll("ntdll.dll", s_ntDllBase);
	GET_FUNCTION(&ntDll, DbgPrint)

	// These allow for cleaner handling of any of these failing
	GET_FUNCTION(&ntDll, NtTerminateProcess)
	GET_FUNCTION(&ntDll, NtRaiseHardError)

	// These are also important
	GET_FUNCTION(&ntDll, NtAllocateVirtualMemory)
	GET_FUNCTION(&ntDll, NtFreeVirtualMemory)
	GET_FUNCTION(&ntDll, RtlAnsiStringToUnicodeString)
	GET_FUNCTION(&ntDll, RtlFreeUnicodeString)

	// ntdll
#ifdef CH_IA32
	GET_FUNCTION(&ntDll, _chkstk)
#else
	GET_FUNCTION(&ntDll, __chkstk)
#endif
	GET_FUNCTION(&ntDll, LdrAddRefDll)
	GET_FUNCTION(&ntDll, LdrGetDllHandle)
	GET_FUNCTION(&ntDll, LdrLoadDll)
	GET_FUNCTION(&ntDll, LdrUnloadDll)
	GET_FUNCTION(&ntDll, NtClose)
	GET_FUNCTION(&ntDll, NtCreateFile)
	GET_FUNCTION(&ntDll, NtCreateMutant)
	GET_FUNCTION(&ntDll, NtCreateKey)
	GET_FUNCTION(&ntDll, NtCreateThread)
	GET_FUNCTION_OPTIONAL(&ntDll, NtCreateThreadEx)
	GET_FUNCTION(&ntDll, NtDelayExecution)
	GET_FUNCTION(&ntDll, NtFreeVirtualMemory)
	GET_FUNCTION(&ntDll, NtProtectVirtualMemory)
	GET_FUNCTION(&ntDll, NtQueryInformationFile)
	GET_FUNCTION(&ntDll, NtQueryInformationThread)
	GET_FUNCTION(&ntDll, NtQuerySystemInformation)
	GET_FUNCTION_OPTIONAL(&ntDll, NtQuerySystemInformationEx)
	GET_FUNCTION(&ntDll, NtQueryValueKey)
	GET_FUNCTION(&ntDll, NtReadFile)
	GET_FUNCTION(&ntDll, NtReleaseMutant)
	GET_FUNCTION(&ntDll, NtResumeThread)
	GET_FUNCTION(&ntDll, NtWaitForSingleObject)
	GET_FUNCTION(&ntDll, NtWriteFile)
	GET_FUNCTION(&ntDll, NtYieldExecution)
	GET_FUNCTION(&ntDll, RtlAddVectoredExceptionHandler)
	GET_FUNCTION(&ntDll, RtlCaptureStackBackTrace)
	GET_FUNCTION(&ntDll, RtlFreeAnsiString)
	GET_FUNCTION(&ntDll, RtlFreeHeap)
	GET_FUNCTION(&ntDll, RtlGetFullPathName_U)
	GET_FUNCTION(&ntDll, RtlInitializeContext)
	GET_FUNCTION(&ntDll, RtlTimeToTimeFields)
	GET_FUNCTION(&ntDll, RtlUnicodeStringToAnsiString)

	// kernelbase is loaded on more OneCore platforms, and exports the functions the engine cares about
	ILibrary* kernel32 = Base_LoadLibrary(AT_LEAST_WINDOWS_VISTA() ? "kernelbase" : "kernel32");

	// kernel32
	// optional because consoles aren't required
	GET_FUNCTION_OPTIONAL(kernel32, AllocConsole)
	GET_FUNCTION_OPTIONAL(kernel32, AttachConsole)
	GET_FUNCTION_OPTIONAL(kernel32, GetConsoleMode)
	GET_FUNCTION_OPTIONAL(kernel32, GetStdHandle)
	GET_FUNCTION_OPTIONAL(kernel32, SetConsoleMode)
	GET_FUNCTION_OPTIONAL(kernel32, WriteConsoleA)
	GET_FUNCTION(kernel32, TlsAlloc)
	GET_FUNCTION(kernel32, TlsGetValue)
	GET_FUNCTION(kernel32, TlsSetValue)

	// shell32
	ILibrary* shell32 = Base_LoadLibrary("shell32");
	if (shell32)
	{
		GET_FUNCTION_OPTIONAL(shell32, SHGetFolderPathA)

		delete shell32;
	}

	// user32
	ILibrary* user32 = Base_LoadLibrary("user32");
	if (user32)
	{
		GET_FUNCTION_OPTIONAL(user32, AdjustWindowRect)
		GET_FUNCTION_OPTIONAL(user32, ClientToScreen)
		GET_FUNCTION_OPTIONAL(user32, CreateWindowExA)
		GET_FUNCTION_OPTIONAL(user32, DefWindowProcA)
		GET_FUNCTION_OPTIONAL(user32, DestroyWindow)
		GET_FUNCTION_OPTIONAL(user32, DispatchMessageA)
		GET_FUNCTION_OPTIONAL(user32, GetClassInfoExA)
		GET_FUNCTION_OPTIONAL(user32, GetClientRect)
		GET_FUNCTION_OPTIONAL(user32, GetDpiForWindow)
		GET_FUNCTION_OPTIONAL(user32, GetSystemMetrics)
		GET_FUNCTION_OPTIONAL(user32, GetWindowLongPtrA)
		GET_FUNCTION_OPTIONAL(user32, IsImmersiveProcess)
		GET_FUNCTION_OPTIONAL(user32, LoadCursorA)
		GET_FUNCTION_OPTIONAL(user32, PeekMessageA)
		GET_FUNCTION_OPTIONAL(user32, RegisterClassExA)
		GET_FUNCTION_OPTIONAL(user32, SetProcessDPIAware)
		GET_FUNCTION_OPTIONAL(user32, SetWindowLongPtrA)
		GET_FUNCTION_OPTIONAL(user32, SetWindowTextA)
		GET_FUNCTION_OPTIONAL(user32, ShowWindow)
		GET_FUNCTION_OPTIONAL(user32, TranslateMessage)
		GET_FUNCTION_OPTIONAL(user32, UnregisterClassA)

		delete user32;
	}

	ILibrary* combase = Base_LoadLibrary("combase");
	if (combase)
	{
		GET_FUNCTION_OPTIONAL(combase, CoTaskMemAlloc)
		GET_FUNCTION_OPTIONAL(combase, RoInitialize)
		GET_FUNCTION_OPTIONAL(combase, RoUninitialize)
		GET_FUNCTION_OPTIONAL(combase, RoGetActivationFactory)
		GET_FUNCTION_OPTIONAL(combase, WindowsCreateString)
		GET_FUNCTION_OPTIONAL(combase, WindowsDeleteString)
		GET_FUNCTION_OPTIONAL(combase, WindowsGetStringRawBuffer)

		delete combase;
	}

	delete kernel32;
#endif

	g_loaderInitialized = true;

	return true;
}

BASEAPI ILibrary* Base_LoadLibrary(cstr name)
{
	static const cstr DLL_EXT =
#ifdef CH_XENON
		".xex";
#else
		".dll";
#endif

#ifdef CH_XENON
	if (g_allocUsable)
	{
		dstr fileName = Base_StrFormat("%s%s", name, DLL_EXT);
		if (!fileName)
		{
			LastNtStatus() = STATUS_NO_MEMORY;
			return nullptr;
		}

		HMODULE xex = LoadLibraryA(fileName);
		Base_Free(fileName);
		if (!xex)
		{
			return nullptr;
		}

		return new CWindowsLibrary(fileName, handle);
	}
#else
	if (STUB_NAME(RtlAnsiStringToUnicodeString) && STUB_NAME(LdrLoadDll) && g_allocUsable)
	{
		dstr fileName = Base_StrFormat("%s%s", name, DLL_EXT);
		if (!fileName)
		{
			LastNtStatus() = STATUS_NO_MEMORY;
			return nullptr;
		}

		if (g_baseInitialized)
		{
			Log_Debug("Loading library %s", fileName);
		}
		else if (DbgPrint_Available())
		{
			DbgPrint("Loading library %s\n", fileName);
		}

		ANSI_STRING nameStr = {};
		nameStr.Buffer = fileName;
		nameStr.Length = (u16)Base_StrLength(fileName);
		nameStr.MaximumLength = nameStr.Length + 1;
		UNICODE_STRING nameUStr = {};
		NTSTATUS status = RtlAnsiStringToUnicodeString(&nameUStr, &nameStr, TRUE);
		Base_Free(fileName);
		if (!NT_SUCCESS(status))
		{
			LastNtStatus() = status;
			return nullptr;
		}

		void* handle = nullptr;
		status = LdrLoadDll(nullptr, nullptr, &nameUStr, &handle);
		if (!NT_SUCCESS(status))
		{
			if (g_baseInitialized)
			{
				Log_Error("Failed to load library %s: NTSTATUS 0x%08X", fileName, status);
			}
			else if (DbgPrint_Available())
			{
				DbgPrint("Failed to load library %s: NTSTATUS 0x%08X\n", fileName, status);
			}
			LastNtStatus() = status;
			RtlFreeUnicodeString(&nameUStr);
			return nullptr;
		}

		RtlFreeUnicodeString(&nameUStr);

		return new CWindowsLibrary(fileName, handle);
	}
#endif

	return nullptr;
}

CWindowsLibrary::CWindowsLibrary(cstr name, void* base) : m_name(nullptr), m_base(base)
{
	// For ntdll, Base_Alloc fails because NtAllocateVirtualMemory hasn't been loaded yet
	if (g_loaderInitialized)
	{
		m_name = Base_StrClone(name);
	}
}

CWindowsLibrary::~CWindowsLibrary()
{
	if (m_name)
	{
		Base_Free(m_name);
	}
}

void CWindowsLibrary::Unload()
{
	if (m_base)
	{
		Log_Debug("Unloading DLL 0x%016zX", reinterpret_cast<uptr>(m_base));
		LdrUnloadDll(m_base);
		m_base = nullptr;
	}
}

void* CWindowsLibrary::GetSymbol(cstr name)
{
#ifdef CH_XENON
	void* sym = GetProcAddress(reinterpret_cast<HMODULE>(m_base), name);
	LastNtStatus() = LastNtError();
	return sym;
#else
	ASSERT_MSG(LdrGetProcedureAddress_Available(), "Base_InitLoader has not been called");

	ANSI_STRING nameStr = {};
	nameStr.Buffer = (dstr)name;
	nameStr.Length = (u16)Base_StrLength(name);
	nameStr.MaximumLength = nameStr.Length + 1;
	void* sym = nullptr;
	NTSTATUS status = LdrGetProcedureAddress(m_base, &nameStr, 0, &sym);
	if (!NT_SUCCESS(status))
	{
		LastNtStatus() = status;
		if (g_baseInitialized)
		{
			Log_Error("Failed to get address of symbol %s: NTSTATUS 0x%08X", name, status);
		}
		else if (DbgPrint_Available())
		{
			DbgPrint("Failed to get address of symbol %s: NTSTATUS 0x%08X\n", name, status);
		}
		return nullptr;
	}

	return sym;
#endif
}
