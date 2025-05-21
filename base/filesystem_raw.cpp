/// \file Raw filesystem
/// \copyright 2025 Randomcode Developers

#include "filesystem_raw.h"
#ifdef CH_WIN32
#include "filesystem_win32.h"
#elif defined CH_XBOX
#include "filesystem_xbox.h"
#elif defined CH_UNIX
#include "filesystem_unix.h"
#elif defined CH_NX
#include "base/filesystem_switch.h"
#endif

CBaseRawFilesystem::CBaseRawFilesystem(cstr root) : m_safe(true)
{
	ssize len = Base_StrLength(root);
	while (len > 0 && (root[len - 1] == '/' || root[len - 1] == '\\'))
	{
		len--;
	}
	m_root = Base_StrClone(root, len);
}

CBaseRawFilesystem::~CBaseRawFilesystem()
{
	Base_Free(m_root);
}

char* CBaseRawFilesystem::Canonicalize(cstr path)
{
	// Check if the path has already been canonicalized
	if (Base_StrCompare(path, m_root, Min(Base_StrLength(path), Base_StrLength(m_root))) == 0)
	{
		return Base_StrClone(path);
	}
	return Base_StrFormat("%s%c%s", m_root, PATH_SEPARATOR, path);
}

BASEAPI IWritableFilesystem* Base_CreateRawFilesystem(cstr path)
{
#ifdef CH_WIN32
	return new CWin32Filesystem(path);
#elif defined CH_XBOX
	return new CXboxFilesystem(path);
#elif defined CH_UNIX
	return new CUnixFilesystem(path);
#elif defined CH_NX
	return new CSwitchFilesystem(path);
#else
	(void)path;
	return nullptr;
#endif
}
