/// \file Basic string functions
/// \copyright Randomcode Developers

#pragma once

#include "base.h"
#include "compiler.h"
#include "types.h"

/// String length
extern BASEAPI ssize Base_StrLength(cstr str, ssize maxSize = SSIZE_MAX);
extern BASEAPI ssize Base_StrLength(cwstr str, ssize maxSize = SSIZE_MAX);

/// Compare two strings
extern BASEAPI s32 Base_StrCompare(cstr RESTRICT a, cstr RESTRICT b, ssize maxCount = SSIZE_MAX, bool caseSensitive = true);

/// Format a string
extern BASEAPI u32 Base_VStrFormat(dstr dest, ssize destSize, cstr format, va_list args);
extern BASEAPI u32 Base_StrFormat(dstr dest, ssize destSize, cstr format, ...);

/// Format a string, and allocate the buffer for it
extern BASEAPI dstr Base_VStrFormat(cstr format, va_list args);
extern BASEAPI dstr Base_StrFormat(cstr format, ...);

/// Copy a string
extern BASEAPI dstr Base_StrCopy(dstr RESTRICT dest, cstr RESTRICT src, ssize maxCount);

/// Duplicate a string
extern BASEAPI dstr Base_StrClone(cstr str, ssize maxSize = SSIZE_MAX);

/// Make a character uppercase
FORCEINLINE char Base_ToUpper(char c)
{
	return c & 0b1011111;
}

/// Make a character lowercase
FORCEINLINE char Base_ToLower(char c)
{
	return c & 0b0100000;
}

/// TODO: Is* functions can probably at least use masks for stuff

/// Check if whitespace
FORCEINLINE bool Base_IsWhitespace(char c)
{
	/// All whitespace characters are in (NUL, space]
	return '\0' < c && c <= ' ';
}

/// Check if ASCII English letter
FORCEINLINE bool Base_IsAlpha(char c)
{
	char upper = Base_ToUpper(c);
	return 'A' <= upper && upper <= 'Z';
}

/// Check if decimal digit
FORCEINLINE bool Base_IsDecDigit(char c)
{
	return '0' <= c && c <= '9';
}

/// Check if letter or number
FORCEINLINE bool Base_IsAlphannumeric(char c)
{
	return Base_IsDecDigit(c) || Base_IsAlpha(c);
}

/// Check if hexadecimal digit
FORCEINLINE bool Base_IsHexDigit(char c)
{
	char upper = Base_ToUpper(c);
	return Base_IsDecDigit(upper) || ('A' <= upper && upper <= 'F');
}

/// Check if is binary digit
FORCEINLINE bool Base_IsBinDigit(char c)
{
	return c == '0' || c == '1';
}

/// Count trailing whitespace
BASEAPI ssize Base_CountTrailingWhitespace(cstr str);

/// Remove trailing whitespace
extern BASEAPI ssize Base_StrTrim(dstr str);
extern BASEAPI dstr Base_StrTrim(cstr str);

/// Find a character in a string
extern BASEAPI ssize Base_StrFind(cstr str, char value, bool reverse = false, ssize maxPos = SSIZE_MAX);

/// Find a string in a string
extern BASEAPI ssize Base_StrFind(cstr str, cstr sequence, bool reverse = false, ssize maxPos = SSIZE_MAX);

/// Format a size
extern BASEAPI CString Base_FormatSize(u64 size);

/// Format a date time
extern BASEAPI CString Base_FormatDateTime(const DateTime_t& time);

/// Parse an integer from a string
extern BASEAPI s64 Base_ParseInt(cstr string, ssize* endOffset = nullptr);

/// Parse a float from a string
extern BASEAPI f64 Base_ParseFloat(cstr string, ssize* endOffset = nullptr);
