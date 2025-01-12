// This file is the reason Base.dll doesn't have an import table. It first locates the base address of ntdll.dll using the
// PEB_LDR_DATA structure, then it parses it and finds the address of LdrGetProcedureAddress by searching the export table for a
// name that matches its hash. Then, it uses LdrGetProcedureAddress to fill in other stubs of system functions, which are
// function pointers that use names the linker expects from an import library, allowing phnt's correct declarations to be used
// without issue. These are basically the same as the import table, but I get to do my own logic for filling them in, so that if
// some function is missing from one version and not another, it isn't a fatal error that can't be avoided, allowing for
// better portability. There are also forwarder functions that are re-exported under the original names so the launcher and video
// system can import the functions.

// On Xbox 360, things are different. xboxkrnl.exe and xam.xex export all the important functions exclusively by ordinal,
// and are in the XEX format. The reason them only using ordinals isn't an issue is because they don't change between kernel
// versions, and it's no longer updated, so that can't change. This means that the logic of this file mostly doesn't apply to the
// 360. Because I don't know how the kernel does things, I've decided that just linking to it is a much better idea.

#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/loader.h"
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

#ifndef CH_XBOX360
// These are desktop/OneCore things

// ntdll
MAKE_STUB(LdrAddDllDirectory, __stdcall, @8)
MAKE_STUB(LdrAddRefDll, __stdcall, @8)
MAKE_STUB(LdrGetProcedureAddress, __stdcall, @16)
MAKE_STUB(LdrLoadDll, __stdcall, @16)
MAKE_STUB(LdrUnloadDll, __stdcall, @4)
MAKE_STUB(NtClose, __stdcall, @4)
MAKE_STUB(NtCreateFile, __stdcall, @44)
MAKE_STUB(NtQueryInformationFile, __stdcall, @20)
MAKE_STUB(NtQuerySystemInformation, __stdcall, @16)
MAKE_STUB(NtRaiseHardError, __stdcall, @0)
MAKE_STUB(NtReadFile, __stdcall, @36)
MAKE_STUB(NtTerminateProcess, __stdcall, @8)
MAKE_STUB(NtWriteFile, __stdcall, @36)
MAKE_STUB(RtlAnsiStringToUnicodeString, __stdcall, @12)
MAKE_STUB(RtlFreeAnsiString, __stdcall, @4)
MAKE_STUB(RtlFreeHeap, __stdcall, @12)
MAKE_STUB(RtlFreeUnicodeString, __stdcall, @4)
MAKE_STUB(RtlGetFullPathName_U, __stdcall, @16)
MAKE_STUB(RtlUnicodeStringToAnsiString, __stdcall, @12)

// Console stuff is all CSR calls I shouldn't rewrite
MAKE_STUB(AllocConsole, __stdcall, @0)
MAKE_STUB(AttachConsole, __stdcall, @4)
MAKE_STUB(GetConsoleMode, __stdcall, @8)
MAKE_STUB(GetStdHandle, __stdcall, @4)
MAKE_STUB(SetConsoleMode, __stdcall, @8)

// shell32
MAKE_STUB(SHGetFolderPathA, __stdcall, @20)

// user32
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
MAKE_STUB(LoadCursorA, __stdcall, @8)
MAKE_STUB(PeekMessageA, __stdcall, @20)
MAKE_STUB(RegisterClassExA, __stdcall, @4)
MAKE_STUB(SetProcessDPIAware, __stdcall, @0)
MAKE_STUB(SetWindowLongPtrA, __stdcall, @12)
MAKE_STUB(SetWindowTextA, __stdcall, @8)
MAKE_STUB(ShowWindow, __stdcall, @8)
MAKE_STUB(TranslateMessage, __stdcall, @4)
MAKE_STUB(UnregisterClassA, __stdcall, @8)
#endif

#ifdef CH_XBOX360
#else
static bool FindNtDll(PPEB_LDR_DATA ldrData)
{
	// On desktop NTDLL is always the first image initialized, no matter what (there is no logical reason this would change)
	// On WoW64, this _does_ get the 32-bit NTDLL, which is the right one
	PLIST_ENTRY ntDllLink = ldrData->InInitializationOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY entry = CONTAINING_STRUCTURE(LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks, ntDllLink);
	s_ntDllBase = static_cast<PIMAGE_DOS_HEADER>(entry->DllBase);

	return s_ntDllBase != nullptr;
}

// Somehow the Rtl function for this isn't inline in phnt, I guess it does that thing with the last section or whatever
#define RVA_TO_VA(base, rva) (reinterpret_cast<u8*>(base) + (rva))

static bool CheckWoW64()
{
#ifdef CH_I386
	PIMAGE_NT_HEADERS ntDllNtHdrs = reinterpret_cast<PIMAGE_NT_HEADERS>(RVA_TO_VA(ntDllBase, ntDllBase->e_lfanew));
	return ntDllNtHdrs->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64;
#else
	return false;
#endif
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
			STUB_NAME(LdrGetProcedureAddress) = (uptr(*)(...))RVA_TO_VA(s_ntDllBase, functions[ordinals[i]]);
			break;
		}
	}

	return LdrGetProcedureAddress_Available();
}
#endif

#define GET_FUNCTION_OPTIONAL(lib, name)                                                                                         \
	STUB_NAME(name) = reinterpret_cast<ILibrary*>(lib)->GetSymbol<uptr (*)(...)>(STRINGIZE(name));
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

#ifndef CH_XBOX360
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
	GET_FUNCTION(&ntDll, LdrAddRefDll)
	GET_FUNCTION(&ntDll, LdrLoadDll)
	GET_FUNCTION(&ntDll, LdrUnloadDll)
	GET_FUNCTION(&ntDll, NtClose)
	GET_FUNCTION(&ntDll, NtCreateFile)
	GET_FUNCTION(&ntDll, NtQueryInformationFile)
	GET_FUNCTION(&ntDll, NtQuerySystemInformation)
	GET_FUNCTION(&ntDll, NtReadFile)
	GET_FUNCTION(&ntDll, NtWriteFile)
	GET_FUNCTION(&ntDll, RtlFreeAnsiString)
	GET_FUNCTION(&ntDll, RtlFreeHeap)
	GET_FUNCTION(&ntDll, RtlGetFullPathName_U)
	GET_FUNCTION(&ntDll, RtlTimeToTimeFields)
	GET_FUNCTION(&ntDll, RtlUnicodeStringToAnsiString)

	// So unloading it when ntDll goes out of scope doesn't mess anything up, cause the loader wasn't used to "load" it
	LdrAddRefDll(0, s_ntDllBase);

	ILibrary* kernel32 = Base_LoadLibrary("kernel32");
	ASSERT(kernel32 != nullptr);

	// kernel32
	GET_FUNCTION(kernel32, AllocConsole)
	GET_FUNCTION(kernel32, AttachConsole)
	GET_FUNCTION(kernel32, GetConsoleMode)
	GET_FUNCTION(kernel32, GetStdHandle)
	GET_FUNCTION(kernel32, SetConsoleMode)

	ILibrary* shell32 = Base_LoadLibrary("shell32");
	ASSERT(shell32 != nullptr);

	// shell32
	GET_FUNCTION(shell32, SHGetFolderPathA)

	ILibrary* user32 = Base_LoadLibrary("user32");
	ASSERT(user32 != nullptr);

	// user32
	GET_FUNCTION(user32, AdjustWindowRect)
	GET_FUNCTION(user32, ClientToScreen)
	GET_FUNCTION(user32, CreateWindowExA)
	GET_FUNCTION(user32, DefWindowProcA)
	GET_FUNCTION(user32, DestroyWindow)
	GET_FUNCTION(user32, DispatchMessageA)
	GET_FUNCTION(user32, GetClassInfoExA)
	GET_FUNCTION(user32, GetClientRect)
	GET_FUNCTION_OPTIONAL(user32, GetDpiForWindow)
	GET_FUNCTION(user32, GetSystemMetrics)
	GET_FUNCTION(user32, GetWindowLongPtrA)
	GET_FUNCTION(user32, LoadCursorA)
	GET_FUNCTION(user32, PeekMessageA)
	GET_FUNCTION(user32, RegisterClassExA)
	GET_FUNCTION_OPTIONAL(user32, SetProcessDPIAware);
	GET_FUNCTION(user32, SetWindowLongPtrA)
	GET_FUNCTION(user32, SetWindowTextA)
	GET_FUNCTION(user32, ShowWindow)
	GET_FUNCTION(user32, TranslateMessage)
	GET_FUNCTION(user32, UnregisterClassA)

	// can't do this cause the dlls would unload and stuff
	// delete kernel32;
	// delete shell32;
	// delete user32;
#endif

	g_loaderInitialized = true;

	return true;
}

BASEAPI ILibrary* Base_LoadLibrary(cstr name)
{
	static const cstr DLL_EXT =
#ifdef CH_XBOX360
		".xex";
#else
		".dll";
#endif

#ifdef CH_XBOX360
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

		CWindowsLibrary* library = new CWindowsLibrary(fileName, handle);

		return library;
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
			LastNtStatus() = status;
			RtlFreeUnicodeString(&nameUStr);
			return nullptr;
		}

		RtlFreeUnicodeString(&nameUStr);

		CWindowsLibrary* library = new CWindowsLibrary(fileName, handle);

		return library;
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
#ifdef CH_XBOX360
	FreeLibrary(reinterpret_cast<HMODULE>(m_base));
#else
	if (LdrLoadDll_Available() && m_base)
	{
		LdrUnloadDll(m_base);
	}
#endif

	if (m_name)
	{
		Base_Free(m_name);
	}
}

void* CWindowsLibrary::GetSymbol(cstr name)
{
#ifdef CH_XBOX360
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
		Log_Error("Failed to get address of symbol %s: NTSTATUS 0x%08X", name, status);
		return nullptr;
	}

	return sym;
#endif
}
