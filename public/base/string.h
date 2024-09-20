// Dynamic array implementation

#pragma once

#include <new>

#include "base.h"
#include "basicstr.h"
#include "container.h"
#include "types.h"

class CString
{
  public:
	CString() : m_buffer(nullptr), m_size(0), m_capacity(0), m_sorted(false)
	{
	}

	// Create a string from the given buffer
	CString(cstr data, ssize size = SSIZE_MAX) : CString()
	{
		size = Min(size, Base_StrLength(data));
		Resize(size);
		Base_MemCopy(m_buffer, data, m_size - 1);
		Terminate();
	}

	// Avoid this
	CString(const CString& other) : CString(other.Data(), other.Length())
	{
	}

	~CString()
	{
		if (m_buffer)
		{
			Base_Free(m_buffer);
		}
	}

	// Get the underlying buffer of this string
	dstr Data() const
	{
		return m_buffer;
	}

	ssize Size() const
	{
		return m_size;
	}

	ssize Length() const
	{
		return m_size - 1;
	}

	// Resize the string to the given size
	void Resize(ssize newSize)
	{
		ssize oldSize = m_size;
		if (m_capacity < newSize)
		{
			Reserve(Max(m_capacity * 2, newSize + 1));
		}
		m_size = newSize + 1;

		if (oldSize < newSize)
		{
			m_sorted = false;
			Terminate();
		}
	}

	// Reserve space for future additions
	void Reserve(ssize size)
	{
		if (size > m_capacity)
		{
			m_capacity = size;
			m_buffer = static_cast<dstr>(Base_Realloc(m_buffer, m_capacity * sizeof(char)));
			ASSERT(m_buffer != nullptr);
		}
	}

	// Add a character
	ssize Add(char character, ssize index = BAD_INDEX)
	{
		m_sorted = false;
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

	// Add another string
	ssize Add(cstr other, ssize index = BAD_INDEX, ssize count = SSIZE_MAX)
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

	// Add the contents of another vector
	ssize Add(const CString& other, ssize index = BAD_INDEX)
	{
		return Add(other.Data(), index, other.Length());
	}

	// Remove the given element
	void Delete(ssize index = BAD_INDEX)
	{
		// Delete the last character by default
		if (index == BAD_INDEX || index >= m_size - 2)
		{
			index = m_size - 2;
		}

		Base_MemCopy(&m_buffer[index], &m_buffer[index + 1], m_size - index - 1);
		m_size--;
		Terminate();
		m_sorted = false;
	}

	// Empty the vector
	void Empty()
	{
		Resize(0);
	}

	// Get the given element
	char operator[](const ssize& index) const
	{
		ASSERT(index < m_size && index >= 0);
		return m_buffer[index];
	}

	// Append an object
	void operator+=(char character)
	{
		Add(character);
	}

	// Append the contents of another string
	void operator+=(const CString& other)
	{
		Add(other);
	}

	// Append the contents of a C string
	void operator+=(cstr other)
	{
		Add(other);
	}

	// Multiply the string
	CString operator*(const ssize& right)
	{
		CString other = *this;
		other.Reserve((m_size - 1) * right + 1);
		for (ssize i = 0; i < right - 1; i++)
		{
			other += *this;
		}

		return other;
	}

	// Compare the string
	bool operator==(cstr other)
	{
		return Base_StrCompare(m_buffer, other) == 0;
	}

	// Compare the string
	bool operator==(const CString& other)
	{
		return *this == other.Data();
	}

	// Not implemented yet
	ssize Find(s32 (*Compare)(char a, char b)) const
	{
		(void)Compare;
		return BAD_INDEX;
	}

  private:
	dstr m_buffer;
	ssize m_size; // Includes the NUL at the end
	ssize m_capacity;
	bool m_sorted;

	static const ssize BAD_INDEX = -1;

	FORCEINLINE void Terminate()
	{
		m_buffer[m_size - 1] = 0;
	}
};
