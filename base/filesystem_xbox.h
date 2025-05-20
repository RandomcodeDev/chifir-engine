/// \file Xbox raw filesystem
/// \copyright 2025 Randomcode Developers

#pragma once

#ifndef CH_XBOX
#error "This header is Xbox only"
#endif

#include "base/compiler.h"
#include "base/filesystem.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"

#include "filesystem_raw.h"

class CXboxFilesystem: public CBaseRawFilesystem
{
  public:
	CXboxFilesystem(cstr root);
	~CXboxFilesystem();

	virtual ssize GetSize(cstr path);
	virtual bool Read(cstr path, CVector<u8>& buffer, ssize count = SSIZE_MAX, ssize offset = 0);
	virtual FileType GetFileType(cstr path);
	virtual bool Exists(cstr path);
	virtual IDirIter* ReadDirectory(cstr path);
	virtual ssize Write(cstr path, const void* data, ssize count, bool append = true, ssize offset = 0);
	virtual bool CreateDirectory(cstr path);

  private:
	HANDLE OpenFile(cstr path, bool writable = false);
	ssize GetSize(HANDLE file);
};
