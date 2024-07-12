// This file is the reason Base.dll doesn't have an import table. It first locates the base address of ntdll.dll using the
// PEB_LDR_DATA structure, then it parses it and finds the address of LdrGetProcedureAddress, by searching the export table by
// hash. Then, it uses LdrGetProcedureAddress to fill in other stubs of system functions, which are function pointers that use
// names the linker expects from an import library, allowing phnt's correct declarations to be used without issue. These are
// basically the same as the import table, but I get to do my own logic for filling them in, so that if some function is missing
// from one version and not another, it can isn't a fatal error that can't be avoided, allowing for better portability. There are
// also forwarder functions that are re-exported under the original names so the launcher and video system can import the
// functions.

// On Xbox 360, things are different. xboxkrnl.exe and xam.xex export all the important functions exclusively by ordinal,
// and are in the XEX format. The reason them only using ordinals isn't an issue is because they don't change between kernel
// versions, and it's no longer updated, so that can't change. This means that the logic of this file mostly doesn't apply to the
// 360, despite it being otherwise quite similar to desktop versions of NT. The one thing that's left is figuring out if ASLR is
// used on the 360, and if so, how to get the kernel's base address without linking to it. I don't know if the PEB exists on the
// 360, and if it does I don't think it's likely required for a reimplementation of this file. I also have to figure out how to
// parse a XEX in memory, but that shouldn't be hard. XAM seems to be the closest analogue to kernel32, but some functions that
// would normally just be thin wrappers or directly forwarder are fully implemented inside of it.

#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/loader.h"
#include "platform_win32.h"

static PIMAGE_DOS_HEADER ntDllBase;

// These are in both Xbox 360 and desktop

// ntdll/xboxkrnl
MAKE_STUB(DbgPrint, __stdcall, 4)
MAKE_STUB(NtAllocateVirtualMemory, __stdcall, 24)
MAKE_STUB(NtFreeVirtualMemory, __stdcall, 16)
MAKE_STUB(RtlAnsiStringToUnicodeString, __stdcall, 12)
MAKE_STUB(RtlFreeUnicodeString, __stdcall, 4)

#ifdef CH_XBOX360
// These are only on Xbox 360

// xboxkrnl
MAKE_STUB(XexGetProcedureAddress)
MAKE_STUB(XexLoadImage)
MAKE_STUB(XexUnloadImage)

// XAM
MAKE_STUB(QueryPerformanceCounter)
#else
// ntdll
MAKE_STUB(LdrAddRefDll, __stdcall, 8)
MAKE_STUB(LdrGetProcedureAddress, __stdcall, 16)
MAKE_STUB(LdrLoadDll, __stdcall, 16)
MAKE_STUB(LdrUnloadDll, __stdcall, 4)
MAKE_STUB(NtQuerySystemInformation, __stdcall, 16)
MAKE_STUB(NtRaiseHardError, __stdcall, 0)
MAKE_STUB(NtTerminateProcess, __stdcall, 8)
MAKE_STUB(RtlFreeHeap, __stdcall, 12)

// user32, because it's more than just syscalls, unlike kernel32
MAKE_STUB(CreateWindowExA, __stdcall, 48)
MAKE_STUB(RegisterClassExA, __stdcall, 4)
MAKE_STUB(PeekMessageA, __stdcall, 20)
MAKE_STUB(TranslateMessage, __stdcall, 4)
MAKE_STUB(DispatchMessageA, __stdcall, 4)
MAKE_STUB(GetClientRect, __stdcall, 8)
MAKE_STUB(GetSystemMetrics, __stdcall, 4)
MAKE_STUB(AdjustWindowRect, __stdcall, 4)
#endif

static bool FindNtDll()
{
	if (ntDllBase)
	{
		return true;
	}

	PPEB_LDR_DATA ldrData = NtCurrentPeb()->Ldr;

	// On desktop NTDLL is always the first image initialized, no matter what (there is no logical reason this would change)
	PLIST_ENTRY ntDllLink = ldrData->InInitializationOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(ntDllLink, LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);
	ntDllBase = (PIMAGE_DOS_HEADER)entry->DllBase;

	return ntDllBase != nullptr;
}

// Somehow the Rtl function for this isn't inline in phnt, I guess it does that thing with the last section or whatever
#define RVA_TO_VA(base, rva) (reinterpret_cast<u8*>(base) + (rva))

static bool FindLdrGetProcedureAddress()
{
	if (STUB_NAME(LdrGetProcedureAddress))
	{
		return true;
	}

	PIMAGE_NT_HEADERS ntHdrs = (PIMAGE_NT_HEADERS)RVA_TO_VA(ntDllBase, ntDllBase->e_lfanew);
	ASSERT(ntHdrs->Signature == IMAGE_NT_SIGNATURE); // Even if this isn't NTDLL, it sure as hell should have the right signature
	ASSERT(ntHdrs->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC);
	ASSERT_MSG(
		ntHdrs->OptionalHeader.SizeOfHeaders >=
			(ntDllBase->e_lfanew + sizeof(IMAGE_NT_SIGNATURE) + sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER) +
			 IMAGE_DIRECTORY_ENTRY_EXPORT * sizeof(IMAGE_DATA_DIRECTORY)),
		"Export directory not present");

	PIMAGE_DATA_DIRECTORY exportDirectory = &ntHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)RVA_TO_VA(ntDllBase, exportDirectory->VirtualAddress);

	u64 nameCount = exports->NumberOfNames;
	u32* nameAddrs = (u32*)RVA_TO_VA(ntDllBase, exports->AddressOfNames);
	u16* ordinals = (u16*)RVA_TO_VA(ntDllBase, exports->AddressOfNameOrdinals);
	u32* functions = (u32*)RVA_TO_VA(ntDllBase, exports->AddressOfFunctions);

	static const u64 TARGET_HASH = 0x49d2dcb66c6c2384;
	for (u64 i = 0; i < nameCount; i++)
	{
		cstr name = (cstr)RVA_TO_VA(ntDllBase, nameAddrs[i]);
		u64 hash = Base_Fnv1a64(name, Base_StrLen(name));
		if (hash == TARGET_HASH) // To get around collisions
		{
			STUB_NAME(LdrGetProcedureAddress) = (uptr(*)(...))RVA_TO_VA(ntDllBase, functions[ordinals[i]]);
			break;
		}
	}

	return STUB_NAME(LdrGetProcedureAddress) != nullptr;
}

#define GET_FUNCTION(lib, name)                                                                                                  \
	{                                                                                                                            \
		STUB_NAME(name) = (reinterpret_cast<ILibrary*>(&(lib)))->GetSymbol<uptr (*)(...)>(#name);                                \
		ASSERT_CODE(STUB_NAME(name) != nullptr, NtCurrentTeb()->LastStatusValue);                                                \
	}

bool Base_InitLoader()
{
	if (!FindNtDll())
	{
		return false;
	}

	if (!FindLdrGetProcedureAddress())
	{
		return false;
	}

	CWin32Library ntDll(ntDllBase);
	GET_FUNCTION(ntDll, NtTerminateProcess); // Make sure if the others fail, they can properly exit

	GET_FUNCTION(ntDll, DbgPrint);
	GET_FUNCTION(ntDll, LdrAddRefDll);
	GET_FUNCTION(ntDll, LdrLoadDll);
	GET_FUNCTION(ntDll, LdrUnloadDll);
	GET_FUNCTION(ntDll, NtAllocateVirtualMemory);
	GET_FUNCTION(ntDll, NtFreeVirtualMemory);
	GET_FUNCTION(ntDll, NtQuerySystemInformation);
	GET_FUNCTION(ntDll, NtRaiseHardError);
	GET_FUNCTION(ntDll, RtlAnsiStringToUnicodeString);
	GET_FUNCTION(ntDll, RtlFreeHeap);
	GET_FUNCTION(ntDll, RtlFreeUnicodeString);

	// So unloading it when ntDll goes out of scope doesn't mess anything up, cause the loader wasn't used to "load" it
	LdrAddRefDll(0, ntDllBase);

	// This doesn't get deleted, which is fine cause it's alive the whole program either way
	ILibrary* user32 = Base_LoadLibrary("user32");

	return true;
}

BASEAPI ILibrary* Base_LoadLibrary(cstr name)
{
	static const cstr DLL_EXT = ".dll";

	if (STUB_NAME(RtlAnsiStringToUnicodeString) && STUB_NAME(LdrLoadDll))
	{
		UNICODE_STRING nameUStr = {};
		ANSI_STRING nameStr = {0};
		nameStr.Buffer = (dstr)name;
		nameStr.Length = (u16)Base_StrLen(name);
		nameStr.MaximumLength = nameStr.Length + 1u;
		bool freeString = NT_SUCCESS(RtlAnsiStringToUnicodeString(&nameUStr, &nameStr, TRUE));

		void* handle = nullptr;
		NTSTATUS status = LdrLoadDll(nullptr, nullptr, &nameUStr, &handle);
		NtCurrentTeb()->LastStatusValue = status;
		if (NT_SUCCESS(status))
		{
			// Don't leave the string dangling
			if (freeString)
			{
				RtlFreeUnicodeString(&nameUStr);
			}

			return new CWin32Library(handle);
		}

		if (freeString)
		{
			RtlFreeUnicodeString(&nameUStr);
		}
	}

	return nullptr;
}

CWin32Library::CWin32Library(void* base) : m_base(base)
{
}

CWin32Library::~CWin32Library()
{
	if (STUB_NAME(LdrUnloadDll) && m_base)
	{
		LdrUnloadDll(m_base);
	}
}

void* CWin32Library::GetSymbol(cstr name)
{
	ASSERT(STUB_NAME(LdrGetProcedureAddress));

	ANSI_STRING nameStr = {0};
	nameStr.Buffer = (dstr)name;
	nameStr.Length = (u16)Base_StrLen(name);
	nameStr.MaximumLength = nameStr.Length + 1u;
	void* sym = nullptr;
	NTSTATUS status = LdrGetProcedureAddress(m_base, &nameStr, 0, &sym);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		return nullptr;
	}

	return sym;
}
