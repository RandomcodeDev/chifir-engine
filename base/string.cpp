
#pragma once

#include "base/string.h"

CString::CString(cstr data, ssize size) : m_buffer(nullptr), m_size(0), m_capacity(0)
{
	size = Min(size, Base_StrLength(data));
	Resize(size);
	Base_MemCopy(m_buffer, data, m_size - 1);
	Terminate();
}

CString::CString(const CString& other) : m_buffer(nullptr), m_size(0), m_capacity(0)
{
	Resize(other.Length());
	Base_MemCopy(m_buffer, other.Data(), m_size - 1);
	Terminate();
}

CString::~CString()
{
	if (m_buffer)
	{
		Base_Free(m_buffer);
	}
}

void CString::Resize(ssize newSize)
{
	ssize oldSize = m_size;
	if (m_capacity < newSize)
	{
		Reserve(Max(m_capacity * 2, newSize + 1));
	}
	m_size = newSize + 1;

	if (oldSize < newSize)
	{
		Terminate();
	}
}

void CString::Reserve(ssize size)
{
	if (size > m_capacity)
	{
		m_capacity = size;
		m_buffer = static_cast<dstr>(Base_Realloc(m_buffer, m_capacity * sizeof(char)));
		ASSERT(m_buffer != nullptr);
	}
}

ssize CString::Add(char character, ssize index)
{
	m_size++;
	if (m_size - 1 > m_capacity)
	{
		Reserve(Max<ssize>(1, m_capacity * 2));
	}

	// Append by default
	if (index == BAD_INDEX || index >= m_size - 2)
	{
		m_buffer[m_size - 2] = character;
		Terminate();
		return m_size - 2;
	}
	else
	{
		Base_MemCopy(m_buffer + index + 1, m_buffer + index, m_size - index - 2);
		m_buffer[index] = character;
		Terminate();
		return index;
	}
}

ssize CString::Add(cstr other, ssize index, ssize count)
{
	count = Min(count, Base_StrLength(other));

	// Append by default
	if (index == BAD_INDEX || index >= m_size - 2)
	{
		index = m_size - 1;
	}

	Resize(m_size + count - 1);

	Base_MemCopy(m_buffer + index + count, m_buffer + index, m_size - index - 1);
	Base_MemCopy(m_buffer + index, other, count);
	Terminate();
	return m_size - 2;
}

void CString::Delete(ssize index, ssize count)
{
	// Delete the last character by default
	if (index == BAD_INDEX || index >= m_size - 2)
	{
		index = m_size - 2;
	}
    if (index + count > m_size - 1)
    {
        count = 1;
    }

	Base_MemCopy(&m_buffer[index], &m_buffer[index + count + 1], m_size - index - count - 1);
	m_size -= count;
	Terminate();
}

CString CString::operator*(const ssize& right)
{
	CString other = *this;
	other.Reserve((m_size - 1) * right + 1);
	for (ssize i = 0; i < right - 1; i++)
	{
		other += *this;
	}

	return other;
}

void CString::VFormat(cstr format, va_list args)
{
	va_list args2;

	va_copy(args2, args);
	ssize size = Base_VStrFormat(nullptr, 0, format, args2);
	Resize(size);
	Base_VStrFormat(m_buffer, static_cast<s32>(m_size), format, args2);
	va_end(args2);
}

ssize CString::Find(s32 (*Compare)(char a, char b)) const
{
	(void)Compare;
	return BAD_INDEX;
}