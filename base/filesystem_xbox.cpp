/// \file Xbox raw filesystem
/// \copyright 2025 Randomcode Developers

#include "filesystem_xbox.h"
#include "base.h"
#include "base/log.h"

CXboxFilesystem::CXboxFilesystem(cstr root) : CBaseRawFilesystem(root)
{
	ASSERT_MSG(g_platInitialized != false, "Call Base_Init first!");

	if (!CreateDirectoryA(root, nullptr))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			Base_Quit("Failed to open directory %s: Win32 error %d", m_root, GetLastError());
		}
	}
}

CXboxFilesystem::~CXboxFilesystem()
{
}

ssize CXboxFilesystem::GetSize(cstr path)
{
	HANDLE file = OpenFile(path);
	if (!file)
	{
		return -1;
	}

	ssize size = GetSize(file);
	CloseHandle(file);

	return size;
}

ssize CXboxFilesystem::GetSize(HANDLE file)
{
	DWORD size = GetFileSize(file, nullptr);
    if (size == UINT32_MAX)
    {
        Log_Error("Failed to get file size: Win32 error %d", GetLastError());
        return -1;
    }

    return size;
}

bool CXboxFilesystem::Read(cstr path, CVector<u8>& buffer, ssize count, ssize offset)
{
    dstr fullPath = Canonicalize(path);
	HANDLE file = OpenFile(fullPath);
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
	SetFilePointerEx(file, largeOffset, nullptr, FILE_BEGIN);
	bool succeeded = ReadFile(file, buffer.Data(), (ULONG)buffer.Size(), 0, nullptr);
	CloseHandle(file);
	if (!succeeded)
	{
		Log_Error(
			"Failed to read %zd bytes from offset 0x%X in %s/%s: Win32 error %d", size, offset, m_root, path, GetLastError());
		buffer.Empty();
		return false;
	}

	return true;
}

FileType CXboxFilesystem::GetFileType(cstr path)
{
	dstr fullPath = Canonicalize(path);
	DWORD attributes = GetFileAttributesA(fullPath);
	Base_Free(fullPath);

	FileType type = FileType::Normal;
	if (attributes & FILE_ATTRIBUTE_DEVICE)
	{
		type = FileType::Device;
	}
	else if (attributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		type = FileType::Directory;
	}
	else if (attributes & FILE_ATTRIBUTE_SYSTEM)
	{
		type = FileType::System;
	}

	return type;
}

bool CXboxFilesystem::Exists(cstr path)
{
	dstr fullPath = Canonicalize(path);
	DWORD attributes = GetFileAttributesA(fullPath);
	Base_Free(fullPath);

	// INVALID_FILE_ATTRIBUTES isn't defined on Xenon
	return attributes != (DWORD)-1;
}

IDirIter* CXboxFilesystem::ReadDirectory(cstr path)
{
	// TODO: implement this
	(void)path;
	return nullptr;
}

ssize CXboxFilesystem::Write(cstr path, const void* data, ssize count, bool append, ssize offset)
{
    dstr fullPath = Canonicalize(path);
	HANDLE file = OpenFile(fullPath, true);
	if (!file)
	{
		return -1;
	}

	if (append)
    {
        offset += GetSize(file);
    }

	SetFilePointer(file, offset, nullptr, FILE_BEGIN);
	if (!WriteFile(file, data, count, nullptr, nullptr))
	{
		m_safe = false;
		Log_Error(
			"Failed to write %zd bytes to %s/%s at offset 0x%X: Win32 error %d", count, m_root, path, offset,
			GetLastError());
		m_safe = true;
		CloseHandle(file);
		return -1;
	}

    FlushFileBuffers(file);

	CloseHandle(file);

	return count;
}

bool CXboxFilesystem::CreateDirectory(cstr path)
{
	if (!CreateDirectoryA(path, nullptr))
	{
		Log_Error("Failed to create directory %s/%s: Win32 error %d", m_root, path, GetLastError());
		return false;
	}

	return true;
}

HANDLE CXboxFilesystem::OpenFile(cstr path, bool writable)
{
	HANDLE file = nullptr;

	ACCESS_MASK access = GENERIC_READ;
	if (writable)
	{
		access |= GENERIC_WRITE;
	}

	file = CreateFileA(
		path, access, FILE_SHARE_READ, nullptr, writable ? OPEN_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (!file)
	{
		m_safe = false;
		Log_Error(
			"Failed to %s %s/%s with access 0x%08X: Win32 error %d", writable ? "create" : "open", m_root, path, access,
			GetLastError());
		m_safe = true;
		return nullptr;
	}

	return file;
}
