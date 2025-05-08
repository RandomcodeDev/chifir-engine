/// \file Windows raw filesystem
/// \copyright 2025 Randomcode Developers

#pragma once

#ifndef CH_WIN32
#error "This header is Windows only"
#endif

#include "base/compiler.h"
#include "base/filesystem.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"
#include "filesystem_raw.h"

class CWin32Filesystem: public CBaseRawFilesystem
{
  public:
	CWin32Filesystem(cstr root);
	~CWin32Filesystem();

	virtual ssize GetSize(cstr path);
	virtual bool Read(cstr path, CVector<u8>& buffer, ssize count = SSIZE_MAX, ssize offset = 0);
	virtual FileType GetFileType(cstr path);
	virtual bool Exists(cstr path);
	virtual IDirIter* ReadDirectory(cstr path);
	virtual ssize Write(cstr path, const void* data, ssize count, bool append = true, ssize offset = 0);
	virtual bool CreateDirectory(cstr path);

  private:
	HANDLE m_rootHandle;

	HANDLE OpenFile(cstr path, bool writable = false);
	ssize GetSize(HANDLE file);
	void ConvertPath(cstr path, PUNICODE_STRING unicodePath);
};
