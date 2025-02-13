#include <cstring>
#include <cerrno>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "base/log.h"
#include "filesystem_unix.h"

CUnixFilesystem::CUnixFilesystem(cstr root) : CBaseRawFilesystem(root)
{
}

CUnixFilesystem::~CUnixFilesystem()
{
}

ssize CUnixFilesystem::GetSize(cstr path)
{
	dstr fullPath = Canonicalize(path);
	struct stat64 st = {};
	stat64(fullPath, &st);
	Base_Free(fullPath);
	return st.st_size;
}

bool CUnixFilesystem::Read(cstr path, CVector<u8>& buffer, ssize count, ssize offset)
{
	int handle = OpenFile(path, false);
	if (handle < 0)
	{
		return false;
	}

	buffer.Resize(count);
	lseek64(handle, offset, SEEK_SET);
	int result = read(handle, buffer.Data(), buffer.Size());
	close(handle);
	if (result < 0)
	{
		Log_Error(
			"Failed to read %zu bytes from offset 0x%X in %s/%s: %s", buffer.Size(), offset, m_root, path,
			strerror(errno));
		buffer.Empty();
		return false;
	}

	return true;
}

FileType_t CUnixFilesystem::GetFileType(cstr path)
{
	dstr fullPath = Canonicalize(path);
	struct stat64 st = {};
	lstat64(fullPath, &st);
	Base_Free(fullPath);
	if (st.st_mode & S_IFREG)
	{
		return FileType_t::Normal;
	}
	else if (st.st_mode & S_IFDIR)
	{
		return FileType_t::Directory;
	}
	else if (st.st_mode & S_IFLNK)
	{
		return FileType_t::Symlink;
	}
	else if ((st.st_mode & S_IFBLK) || (st.st_mode & S_IFCHR))
	{
		return FileType_t::Device;
	}
	else
	{
		return FileType_t::Unknown;
	}
}

bool CUnixFilesystem::Exists(cstr path)
{
	dstr fullPath = Canonicalize(path);
	struct stat64 st = {};
	int result = stat64(fullPath, &st);
	Base_Free(fullPath);
	return result >= 0;
}

IDirIter* CUnixFilesystem::ReadDirectory(cstr path)
{
	(void)path;
	return nullptr;
}

ssize CUnixFilesystem::Write(cstr path, const void* data, ssize count, bool append, ssize offset)
{
	int handle = OpenFile(path, true);
	if (handle < 0)
	{
		return -1;
	}

	if (append)
	{
		offset += GetSize(handle);
	}

	int result = write(handle, data, count);
	close(handle);
	if (result < 0)
	{
		m_safe = false;
		Log_Error("Failed to write %zu bytes to %s/%s at offset 0x%X: %s", count, m_root, path, offset, strerror(errno));
		m_safe = true;
		return -1;
	}

	return count;
}

bool CUnixFilesystem::CreateDirectory(cstr path)
{
	(void)path;
	return false;
}

int CUnixFilesystem::OpenFile(cstr path, bool writable)
{
	dstr fullPath = Canonicalize(path);
	int handle = open(fullPath, O_CREAT);
	Base_Free(fullPath);
	return handle;
}

ssize CUnixFilesystem::GetSize(int file)
{
	struct stat64 st = {};
	fstat64(file, &st);
	return st.st_size;
}
