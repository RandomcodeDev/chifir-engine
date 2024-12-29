// Dynamic string

#pragma once

#include "base.h"
#include "basicstr.h"
#include "container.h"
#include "types.h"

template <typename T> class CVector;

class BASEAPI CString
{
  public:
	CString() : m_buffer(nullptr), m_size(0), m_capacity(0)
	{
	}

	// Create a string from the given buffer
	CString(cstr data, ssize size = SSIZE_MAX);

	// Avoid this, it's expensive
	CString(const CString& other);

	~CString();

	// Get the underlying buffer of this string
	dstr Data() const
	{
		return m_buffer;
	}

	// Get the number of bytes in the string
	ssize Size() const
	{
		return m_size;
	}

	// Get the number of characters not including the NUL
	ssize Length() const
	{
		return m_size - 1;
	}

	// Resize the string to the given size
	void Resize(ssize newSize);

	// Reserve space for future additions
	void Reserve(ssize size);

	// Add a character
	ssize Add(char character, ssize index = BAD_INDEX);

	// Add another string
	ssize Add(cstr other, ssize index = BAD_INDEX, ssize count = SSIZE_MAX);

	// Add the contents of another string
	ssize Add(const CString& other, ssize index = BAD_INDEX)
	{
		return Add(other.Data(), index, other.Length());
	}

	// Remove the given element(s)
	void Delete(ssize index = BAD_INDEX, ssize count = 1);

	// Empty the string
	void Clear()
	{
		Resize(0);
	}

	// Get the given element
	char operator[](const ssize& index) const
	{
		ASSERT(index < m_size && index >= 0);
		return m_buffer[index];
	}

	// Combine two strings
	CString operator+(const CString& right)
	{
		CString str(*this);
		str += right;
		return str;
	}

	// Combine two strings
	CString operator+(char right)
	{
		CString str(*this);
		str += right;
		return str;
	}

	// Combine two strings
	CString operator+(cstr right)
	{
		CString str(*this);
		str += right;
		return str;
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
	CString operator*(const ssize& right);

	// Compare the string
	bool operator==(cstr other) const
	{
		return Base_StrCompare(m_buffer, other) == 0;
	}

	// Compare the string
	bool operator==(const CString& other) const
	{
		// call operator==(cstr other)
		return *this == other.Data();
	}

	// Format into the string
	void VFormat(cstr format, va_list args);

	// Format into the string
	void Format(cstr format, ...)
	{
		va_list args;
		va_start(args, format);
		VFormat(format, args);
		va_end(args);
	}

	// Format into a new string
	static CString VFormatStr(cstr format, va_list args)
	{
		CString string;
		va_list args2;
		va_copy(args2, args);
		string.VFormat(format, args2);
		va_end(args2);
		return string;
	}

	// Format into a new string
	static CString FormatStr(cstr format, ...)
	{
		CString string;
		va_list args;
		va_start(args, format);
		string.VFormat(format, args);
		va_end(args);
		return string;
	}

	// Find a character
	ssize Find(char c, bool reverse = false, ssize offset = 0) const
	{
		// TODO: add offset to Base_MemFind and Base_StrFind, this is kinda ugly
		offset = Min(offset, m_size - 1);
		ssize subOffset = Base_StrFind(m_buffer + offset, c, reverse, m_size - offset - 1);
		return subOffset < 0 ? -1 : subOffset + offset;
	}

	// Find a substring
	ssize Find(cstr str, bool reverse = false, ssize offset = 0) const
	{
		offset = Min(offset, m_size - 1);
		return Base_StrFind(m_buffer + offset, str, reverse, m_size - offset - 1);
	}
	
	// Get a substring
	CString SubString(ssize start = 0, ssize count = -1) const
	{
		count = Clamp<ssize>(count, 0, m_size - 1);
		start = Clamp<ssize>(start, 0, m_size - count - 1);

		return CString(m_buffer + start, count);
	}

	// Split the string at each occurence of a character
	CVector<CString> Split(char c = ' ') const;

  private:
	dstr m_buffer;
	ssize m_size; // Includes the NUL at the end
	ssize m_capacity;

	static const ssize BAD_INDEX = -1;

	void Terminate()
	{
		m_buffer[m_size - 1] = 0;
	}
};
