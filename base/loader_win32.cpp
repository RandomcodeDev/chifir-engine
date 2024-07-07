#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/loader.h"
#include "platform_win32.h"

static PIMAGE_DOS_HEADER ntDllBase;

MAKE_STUB(DbgPrint, __stdcall, 4)
MAKE_STUB(LdrAddRefDll, __stdcall, 8)
MAKE_STUB(LdrGetProcedureAddress, __stdcall, 16)
MAKE_STUB(LdrLoadDll, __stdcall, 16)
MAKE_STUB(LdrUnloadDll, __stdcall, 4)
MAKE_STUB(NtAllocateVirtualMemory, __stdcall, 24)
MAKE_STUB(NtFreeVirtualMemory, __stdcall, 16)
MAKE_STUB(NtQuerySystemInformation, __stdcall, 16)
MAKE_STUB(NtRaiseHardError, __stdcall, 0)
MAKE_STUB(NtTerminateProcess, __stdcall, 8)
MAKE_STUB(RtlAnsiStringToUnicodeString, __stdcall, 12)
MAKE_STUB(RtlFreeHeap, __stdcall, 12)
MAKE_STUB(RtlFreeUnicodeString, __stdcall, 4)

static bool FindNtDll()
{
	if (ntDllBase)
	{
		return true;
	}

	PPEB_LDR_DATA ldrData = NtCurrentPeb()->Ldr;

	// NTDLL is always the first image initialized, no matter what (there is no logical reason this would change)
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

	static const u64 LDRGETPROCADDR_XXHASH = 0x49d2dcb66c6c2384; // FNV-1a 64-bit

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

	return true;
}

BASEAPI ILibrary* Base_LoadLibrary(cstr name)
{
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
