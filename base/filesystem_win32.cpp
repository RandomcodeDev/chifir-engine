#include "filesystem_win32.h"
#include "base/log.h"

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
	if (!NT_SUCCESS(status))
	{
		Log_Error("Failed to read %zu bytes from offset %#X in %s/%s: NTSTATUS %#08X", size, offset, m_root, path, status);
		buffer.Empty();
		NtClose(file);
		return false;
	}

	NtClose(file);

	return true;
}

FileType_t CWin32Filesystem::GetFileType(cstr path)
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
		Log_Error("Failed to get attributes of %s/%s: NTSTATUS %#08X", m_root, path, status);
		return FileTypeUnknown;
	}

	FileType_t type = FileTypeNormal;
	if (info.FileAttributes & FILE_ATTRIBUTE_DEVICE)
	{
		type = FileTypeDevice;
	}
	else if (info.FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		type = FileTypeDirectory;
	}
#ifndef CH_XBOX360
	else if (info.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
	{
		type = FileTypeSymlink;
	}
#endif
	else if (info.FileAttributes & FILE_ATTRIBUTE_SYSTEM)
	{
		type = FileTypeSystem;
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
		Log_Error("Failed to get attributes of %s/%s: NTSTATUS %#08X", m_root, path, status);
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

ssize CWin32Filesystem::Write(cstr path, const u8* data, ssize count, bool append, ssize offset)
{
	(void)(path, data, count, append, offset);
	return -1;
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

	dstr fullPath = Canonicalize(path);
	UNICODE_STRING unicodePath = {};
	ConvertPath(fullPath, &unicodePath);
	Base_Free(fullPath);

	OBJECT_ATTRIBUTES objAttrs = RTL_CONSTANT_OBJECT_ATTRIBUTES(&unicodePath, 0);

	ACCESS_MASK access = GENERIC_READ;
	if (writable)
	{
		access |= GENERIC_WRITE;
	}

	NTSTATUS status = NtCreateFile(
		&file, access, &objAttrs, &ioStatus, nullptr, FILE_ATTRIBUTE_NORMAL, 0, writable ? FILE_OPEN_IF : FILE_OPEN, 0, nullptr,
		0);
	RtlFreeUnicodeString(&unicodePath);
	if (!NT_SUCCESS(status))
	{
		Log_Error("Failed to %s %s/%s: NTSTATUS %#08X", writable ? "create" : "open", m_root, path, status);
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
