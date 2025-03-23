/// Raw filesystem
/// Copyright 2025 Randomcode Developers

#pragma once

#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/filesystem.h"
#include "base/types.h"
#include "base/vector.h"

// Raw filesystem that operates on normal files
class CBaseRawFilesystem: public IWritableFilesystem
{
  public:
	CBaseRawFilesystem(cstr root);
	~CBaseRawFilesystem();

	virtual ssize GetSize(cstr path) = 0;
	virtual bool Read(cstr path, CVector<u8>& buffer, ssize count = SSIZE_MAX, ssize offset = 0) = 0;
	virtual FileType_t GetFileType(cstr path) = 0;
	virtual bool Exists(cstr path) = 0;
	virtual IDirIter* ReadDirectory(cstr path) = 0;

	virtual cstr GetRoot()
	{
		return m_root;
	}

	virtual dstr Canonicalize(cstr path);

	virtual ssize Write(cstr path, const void* data, ssize count, bool append = true, ssize offset = 0) = 0;
	virtual bool CreateDirectory(cstr path) = 0;

	virtual bool IsWriteSafe()
	{
		return m_safe;
	}

  protected:
	dstr m_root;
	bool m_safe;
};
