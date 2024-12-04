// Dynamic string

#pragma once

#include "base.h"
#include "basicstr.h"
#include "container.h"
#include "types.h"

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
	CString operator*(const ssize& right);

	// Compare the string
	bool operator==(cstr other)
	{
		return Base_StrCompare(m_buffer, other) == 0;
	}

	// Compare the string
	bool operator==(const CString& other)
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
		va_list args;
		va_start(args, format);
		CString string = VFormatStr(format, args);
		va_end(args);
		return string;
	}

	// Not implemented yet
	ssize Find(s32 (*Compare)(char a, char b)) const;

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
