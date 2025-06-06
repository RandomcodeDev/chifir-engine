/// \file Filesystem interfaces
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/base.h"
#include "base/compiler.h"
#include "base/types.h"
#include "base/vector.h"

#if defined CH_WIN32 || defined CH_XBOX || defined CH_XENON
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

/// forward decl so it can reference FileType_t
class IDirIter;

/// Type of a file
enum class FileType : s32
{
	Normal = 0,
	Directory,
	Symlink,
	System,
	Device,
	Unknown
};

/// Represents a read-only filesystem
class IFilesystem
{
  public:
	virtual ~IFilesystem() = default;

	/// Get the size of a file
	virtual ssize GetSize(cstr path) = 0;

	/// Read a file
	virtual bool Read(cstr path, CVector<u8>& buffer, ssize count = SSIZE_MAX, ssize offset = 0) = 0;

	/// Get the type of a file
	virtual FileType GetFileType(cstr path) = 0;

	/// Get whether a file exists
	virtual bool Exists(cstr path) = 0;

	/// Read a directory
	virtual IDirIter* ReadDirectory(cstr path) = 0;

	/// Get the filesystem's root
	virtual cstr GetRoot() = 0;

	/// Canonicalize a path to this filesystem's root
	virtual dstr Canonicalize(cstr path) = 0;
};

/// A directory iterator
class IDirIter
{
  public:
	virtual ~IDirIter() = default;

	/// Get the path of this entry
	virtual cstr GetPath() = 0;

	/// Get the type of entry this is
	virtual FileType GetType() = 0;
};

/// Represents a read-write filesystem
class IWritableFilesystem: public IFilesystem
{
  public:
	virtual ~IWritableFilesystem() = default;

	/// Write a file
	virtual ssize Write(cstr path, const void* data, ssize count, bool append = true, ssize offset = 0) = 0;

	/// Create a directory
	virtual bool CreateDirectory(cstr path) = 0;

	/// Check if writing is safe (i.e. to stop recursion in logging if a write doesn't work)
	virtual bool IsWriteSafe() = 0;
};

/// Create a raw filesystem (like directly using OS file APIs)
extern BASEAPI IWritableFilesystem* Base_CreateRawFilesystem(cstr path);
