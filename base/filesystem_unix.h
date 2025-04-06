/// \file Unix raw filesystem
/// \copyright Randomcode Developers

#pragma once

#ifndef CH_UNIX
#error "This header is Unix only"
#endif

#include "../base/filesystem_raw.h"
#include "base/compiler.h"
#include "base/filesystem.h"
#include "base/platform.h"
#include "base/string.h"
#include "base/types.h"
#include "base/vector.h"

class CUnixFilesystem: public CBaseRawFilesystem
{
  public:
	CUnixFilesystem(cstr root);
	~CUnixFilesystem();

	ssize GetSize(cstr path);
	bool Read(cstr path, CVector<u8>& buffer, ssize count = SSIZE_MAX, ssize offset = 0);
	FileType GetFileType(cstr path);
	bool Exists(cstr path);
	IDirIter* ReadDirectory(cstr path);
	ssize Write(cstr path, const void* data, ssize count, bool append = true, ssize offset = 0);
	bool CreateDirectory(cstr path);

  private:
	int OpenFile(cstr path, bool writable = false);
	ssize GetSize(int file);
};
