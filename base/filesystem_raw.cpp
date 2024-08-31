#include "filesystem_raw.h"

CBaseRawFilesystem::CBaseRawFilesystem(cstr root)
{
	ssize len = Base_StrLength(root);
	while (len > 0 && root[len - 1] == '/' || root[len - 1] == '\\')
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
	return Base_StrFormat("%s/%s", m_root, path);
}
