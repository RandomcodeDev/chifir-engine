// Dynamic string implementation

#pragma once

#include "base.h"
#include "basicstr.h"
#include "types.h"
#include "vector.h"

#include "stb.h"

class CString : public CVector<char>
{
  public:
	CString() : CVector<char>()
	{
	}

	CString(cstr data, ssize size) : CVector<char>(data, size)
	{
	}

	// Get a C string
	cstr GetCString()
	{
		if (Base_StrCompare(m_buffer, m_cStrBuffer, Min(Size(), stbds_arrlenu(m_cStrBuffer))) != 0)
		{
			stbds_arrsetlen(m_cStrBuffer, Size() + 1);
			Base_StrCopy(m_cStrBuffer, m_buffer, Size());
		}
		return m_cStrBuffer;
	}

	// Append a string
	void operator+=(cstr other)
	{
		for (ssize i = 0; i < Base_StrLength(other); i++)
		{
			Add(other[i]);
		}
	}

	// Get a substring
	CString GetSubString(ssize index, ssize count = SSIZE_MAX) const
	{
		ASSERT(index < Size());
		if (index + count > Size())
		{
			count = Size() - index;
		}
		return CString(m_buffer + index, count);
	}

  private:
	// Buffer where the C string representation is stored
	dstr m_cStrBuffer;
};
