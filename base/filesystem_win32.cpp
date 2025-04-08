/// \file Windows raw filesystem
/// \copyright Randomcode Developers

#include "base.h"
#include "filesystem_win32.h"
#include "base/log.h"

CWin32Filesystem::CWin32Filesystem(cstr root) : CBaseRawFilesystem(root)
{
	ASSERT(g_platInitialized != false);

#ifdef CH_XBOX360
	m_rootHandle = CreateFileA(
		m_root, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, FILE_OPEN_IF,
		FILE_ATTRIBUTE_DIRECTORY, nullptr);
	if (!m_rootHandle)
	{
		Base_Quit("Failed to open directory %s: NTSTATUS 0x%08X", m_root, LastNtStatus());
	}
#else
	UNICODE_STRING rootString;
	UNICODE_STRING unicodeRoot = {};
	ConvertPath(m_root, &unicodeRoot);
	u32 rootLength = RtlGetFullPathName_U(unicodeRoot.Buffer, 0, nullptr, nullptr);
	rootLength += 4 * sizeof(wchar_t); // for \??\ and NUL
	dwstr fullRoot = Base_Alloc<wchar_t>(rootLength);
	if (!fullRoot)
	{
		Base_Quit("Failed to allocate %u wchars for path %s!", rootLength, m_root);
	}

	RtlGetFullPathName_U(unicodeRoot.Buffer, rootLength - 4 * sizeof(wchar_t), fullRoot + 4, nullptr);

	fullRoot[0] = L'\\';
	fullRoot[1] = L'?';
	fullRoot[2] = L'?';
	fullRoot[3] = L'\\';
	rootString.Buffer = fullRoot;
	rootString.Length = static_cast<u16>(rootLength - (1 * sizeof(wchar_t)));
	rootString.MaximumLength = static_cast<u16>(rootLength);

	// Open or create the root
	OBJECT_ATTRIBUTES objAttrs = RTL_CONSTANT_OBJECT_ATTRIBUTES(&rootString, OBJ_CASE_INSENSITIVE);
	IO_STATUS_BLOCK ioStatus = {};
	NTSTATUS status = NtCreateFile(
		&m_rootHandle, GENERIC_READ | GENERIC_WRITE, &objAttrs, &ioStatus, nullptr, FILE_ATTRIBUTE_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN_IF, FILE_DIRECTORY_FILE, nullptr, 0);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		Base_Quit("Failed to open directory %s (%ls): NTSTATUS 0x%08X", m_root, rootString.Buffer, status);
	}

	Base_Free(fullRoot);
#endif
}

CWin32Filesystem::~CWin32Filesystem()
{
	NtClose(m_rootHandle);
}

ssize CWin32Filesystem::GetSize(cstr path)
{
	HANDLE file = OpenFile(path);
	if (!file)
	{
		return -1;
	}

	ssize size = GetSize(file);
	NtClose(file);

	return size;
}

ssize CWin32Filesystem::GetSize(HANDLE file)
{
	IO_STATUS_BLOCK ioStatus = {};
	FILE_STANDARD_INFORMATION info = {};
	NTSTATUS status = NtQueryInformationFile(file, &ioStatus, &info, SIZEOF(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		return -1;
	}

	return (ssize)info.EndOfFile.QuadPart;
}

bool CWin32Filesystem::Read(cstr path, CVector<u8>& buffer, ssize count, ssize offset)
{
	HANDLE file = OpenFile(path);
	if (!file)
	{
		return false;
	}

	ssize size = GetSize(file);
	size -= offset;
	if (count < size)
	{
		size = count;
	}
	buffer.Resize(size);

	LARGE_INTEGER largeOffset = {};
	largeOffset.QuadPart = offset;
	IO_STATUS_BLOCK ioStatus = {};
	NTSTATUS status =
		NtReadFile(file, nullptr, nullptr, nullptr, &ioStatus, buffer.Data(), (ULONG)buffer.Size(), &largeOffset, nullptr);
	NtClose(file);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		Log_Error("Failed to read %zd bytes from offset 0x%X in %s/%s: NTSTATUS 0x%08X", size, offset, m_root, path, status);
		buffer.Empty();
		return false;
	}

	return true;
}

FileType CWin32Filesystem::GetFileType(cstr path)
{
	UNICODE_STRING unicodePath = {};
	ConvertPath(path, &unicodePath);
	OBJECT_ATTRIBUTES objAttrs = {};
	InitializeObjectAttributes(&objAttrs, &unicodePath, OBJ_CASE_INSENSITIVE, &m_rootHandle, nullptr);
	FILE_NETWORK_OPEN_INFORMATION info = {};
	NTSTATUS status = NtQueryFullAttributesFile(&objAttrs, &info);
	RtlFreeUnicodeString(&unicodePath);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		Log_Error("Failed to get attributes of %s/%s: NTSTATUS 0x%08X", m_root, path, status);
		return FileType::Unknown;
	}

	FileType type = FileType::Normal;
	if (info.FileAttributes & FILE_ATTRIBUTE_DEVICE)
	{
		type = FileType::Device;
	}
	else if (info.FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		type = FileType::Directory;
	}
#ifndef CH_XBOX360
	else if (info.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
	{
		type = FileType::Symlink;
	}
#endif
	else if (info.FileAttributes & FILE_ATTRIBUTE_SYSTEM)
	{
		type = FileType::System;
	}

	return type;
}

bool CWin32Filesystem::Exists(cstr path)
{
	dstr fullPath = Canonicalize(path);
	UNICODE_STRING unicodePath = {};
	ConvertPath(fullPath, &unicodePath);
	Base_Free(fullPath);
	OBJECT_ATTRIBUTES objAttrs = RTL_CONSTANT_OBJECT_ATTRIBUTES(&unicodePath, 0);
	FILE_NETWORK_OPEN_INFORMATION info = {};
	NTSTATUS status = NtQueryFullAttributesFile(&objAttrs, &info);
	RtlFreeUnicodeString(&unicodePath);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		Log_Error("Failed to get attributes of %s/%s: NTSTATUS 0x%08X", m_root, path, status);
		return false;
	}

	// INVALID_FILE_ATTRIBUTES = (DWORD)-1, but 360 doesn't have it defined
	return info.FileAttributes != (u32)-1;
}

IDirIter* CWin32Filesystem::ReadDirectory(cstr path)
{
	// TODO: implement this
	(void)path;
	return nullptr;
}

ssize CWin32Filesystem::Write(cstr path, const void* data, ssize count, bool append, ssize offset)
{
	HANDLE file = OpenFile(path, true);
	if (!file)
	{
		return -1;
	}

	IO_STATUS_BLOCK ioStatus = {};
	LARGE_INTEGER largeOffset = {};
	largeOffset.QuadPart = offset;
	if (append)
	{
		largeOffset.QuadPart += GetSize(file);
	}

	// NtWriteFile better not modify the buffer
	NTSTATUS status = NtWriteFile(
		file, nullptr, nullptr, nullptr, &ioStatus, CONST_CAST(void*, data), static_cast<u32>(count), &largeOffset, nullptr);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		m_safe = false;
		Log_Error(
			"Failed to write %zd bytes to %s/%s at offset 0x%X: NTSTATUS 0x%08X", count, m_root, path, largeOffset.QuadPart,
			status);
		m_safe = true;
		NtClose(file);
		return -1;
	}

	NtClose(file);

	return ioStatus.Information;
}

bool CWin32Filesystem::CreateDirectory(cstr path)
{
	(void)path;
	return false;
}

HANDLE CWin32Filesystem::OpenFile(cstr path, bool writable)
{
	HANDLE file = nullptr;
	IO_STATUS_BLOCK ioStatus = {};

	UNICODE_STRING unicodePath = {};
	ConvertPath(path, &unicodePath);

	OBJECT_ATTRIBUTES objAttrs = {};
	InitializeObjectAttributes(&objAttrs, &unicodePath, 0, m_rootHandle, nullptr);

	ACCESS_MASK access = GENERIC_READ;
	if (writable)
	{
		access |= GENERIC_WRITE;
	}

	NTSTATUS status = NtCreateFile(
		&file, access, &objAttrs, &ioStatus, nullptr, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, writable ? FILE_OPEN_IF : FILE_OPEN,
		0, nullptr, 0);
	RtlFreeUnicodeString(&unicodePath);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		m_safe = false;
		Log_Error(
			"Failed to %s %s/%s with access 0x%08X: NTSTATUS 0x%08X", writable ? "create" : "open", m_root, path, access, status);
		m_safe = true;
		return nullptr;
	}

	return file;
}

void CWin32Filesystem::ConvertPath(cstr path, PUNICODE_STRING unicodePath)
{
	ANSI_STRING ansiPath = {};
	ansiPath.Buffer = (dstr)path;
	ansiPath.Length = (USHORT)Base_StrLength(path);
	ansiPath.MaximumLength = ansiPath.Length + 1;
	RtlAnsiStringToUnicodeString(unicodePath, &ansiPath, true);
}
