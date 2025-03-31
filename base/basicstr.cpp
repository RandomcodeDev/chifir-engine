/// \file String functions
/// \copyright 2025 Randomcode Developers

#include "base/basicstr.h"
#include "base/base.h"
#include "base/compiler.h"
#include "base/dll.h"
#include "base/string.h"
#include "base/types.h"

BASEAPI ssize Base_StrLength(cstr str, ssize maxSize)
{
	return Base_MemFind(str, maxSize < 0 ? SSIZE_MAX : maxSize, '\0', false);
}

BASEAPI ssize Base_StrLength(cwstr str, ssize maxSize)
{
	return (Base_MemFind(str, maxSize < 0 ? SSIZE_MAX : maxSize, L"", sizeof(wchar_t), false) + 1) / sizeof(wchar_t);
}

BASEAPI s32 Base_StrCompare(cstr RESTRICT a, cstr RESTRICT b, ssize maxCount, bool caseSensitive)
{
	ssize size = Min(Base_StrLength(a), maxCount);

	if (caseSensitive)
	{
		return Base_MemCompare(a, b, size);
	}
	else
	{
		for (ssize i = 0; i < size; i++)
		{
			char ac = Base_ToUpper(a[i]);
			char bc = Base_ToUpper(b[i]);
			if (ac < bc)
			{
				return -1;
			}
			else if (ac > bc)
			{
				return 1;
			}
		}

		return 0;
	}
}

BASEAPI dstr Base_VStrFormat(cstr format, va_list args)
{
	va_list newArgs;

	va_copy(newArgs, args);
	// Size of string plus nul terminator
	u32 size = Base_VStrFormat(nullptr, 0, format, newArgs) + 1;
	dstr buf = Base_Alloc<char>(size);
	if (!buf)
	{
		va_end(args);
		return nullptr;
	}
	Base_VStrFormat(buf, size, format, newArgs);
	va_end(args);

	return buf;
}

BASEAPI dstr Base_StrFormat(cstr format, ...)
{
	va_list args;

	va_start(args, format);
	dstr buf = Base_VStrFormat(format, args);
	va_end(args);

	return buf;
}

BASEAPI dstr Base_StrCopy(dstr RESTRICT dest, cstr RESTRICT src, ssize maxCount)
{
	ssize size = Min(Base_StrLength(src), maxCount);
	if (maxCount < 0)
	{
		size = 1;
	}
	Base_MemCopy(dest, src, size);
	dest[size] = '\0';
	return dest;
}

BASEAPI dstr Base_StrClone(cstr str, ssize maxSize)
{
	if (!str)
	{
		str = "";
	}

	ssize size = Min(Base_StrLength(str), maxSize) + 1;
	if (maxSize < 0)
	{
		size = 1;
	}
	dstr buffer = Base_Alloc<char>(size);
	if (!buffer)
	{
		return nullptr;
	}

	return Base_StrCopy(buffer, str, size);
}

BASEAPI ssize Base_CountTrailingWhitespace(cstr str)
{
	ssize count = 0;
	ssize size = Base_StrLength(str);
	for (ssize i = size - 1; Base_IsWhitespace(str[i]) && i >= 0; i--)
	{
		count++;
	}

	return count;
}

BASEAPI ssize Base_StrTrim(dstr str)
{
	ssize count = Base_CountTrailingWhitespace(str);
	ssize size = Base_StrLength(str);
	Base_MemSet(str + size - count - 1, 0, count);

	return size;
}

BASEAPI dstr Base_StrTrim(cstr str)
{
	ssize count = Base_CountTrailingWhitespace(str);
	ssize size = Base_StrLength(str);
	return Base_StrClone(str, size - count - 1);
}

BASEAPI ssize Base_StrFind(cstr str, char value, bool reverse, ssize maxSize)
{
	return Base_MemFind(str, Min(Base_StrLength(str), maxSize), value, reverse);
}

BASEAPI ssize Base_StrFind(cstr str, cstr sequence, bool reverse, ssize maxSize)
{
	return Base_MemFind(str, Min(Base_StrLength(str), maxSize), sequence, Base_StrLength(sequence) - 1, reverse);
}

BASEAPI CString Base_FormatSize(u64 size)
{
	static const cstr SUFFIXES[] = {
		"B", "kiB", "MiB", "GiB", "TiB", "PiB (damn)", "EiB (are you sure?)",
		// NOTE: these don't all go in increments of 1024, but they're physically
		// impossible for at least another 50 years or whatever and here as a joke anyway
		"ZiB (who are you?)", "YiB (what are you doing?)", "RiB (why are you doing this?)", "QiB (HOW ARE YOU DOING THIS?)",
		"?B (what did you do?)"};

	f64 value = static_cast<f64>(size);
	ssize suffix = 0;
	while (value >= 1024.0)
	{
		value /= 1024.0;
		suffix++;
	}

	// TODO: handle 69, 420, and pi
	return CString::FormatStr("%lf %s", value, SUFFIXES[Min(suffix, ArraySize(SUFFIXES) - 1)]);
}

BASEAPI CString Base_FormatDateTime(const DateTime_t& time)
{
	// https://xkcd.com/1179/
	return CString::FormatStr(
		"%04u-%02u-%02u %02u:%02u:%02u.%03u", time.year, time.month, time.day, time.hour, time.minute, time.second,
		time.millisecond);
}

BASEAPI s64 Base_ParseInt(cstr str, ssize* endOffset)
{
	if (!str)
	{
		if (endOffset)
		{
			*endOffset = -1;
		}
		return 0;
	}

	bool negative = str[0] == '-';
	s64 sign = negative ? -1 : 1;
	ssize start = (str[0] == '-' || str[0] == '+') ? 1 : 0;

	u32 base = 10;
	ssize len = Base_StrLength(str);
	if (len > start + 2)
	{
		switch (str[start + 1])
		{
		case 'b':
		case 'B':
			base = 16;
			start++;
			break;
		case 'x':
		case 'X':
			base = 2;
			start++;
			break;
		}
	}

	s64 value = 0;
	switch (base)
	{
	default:
	case 10: {
		for (ssize i = start; i < len; i++)
		{
			char c = str[i];
			if (!Base_IsDecDigit(c))
			{
				if (endOffset)
				{
					*endOffset = i;
				}
				goto Done;
			}

			s64 digit = c - '0';
			value = value * 10 + digit;
		}
		break;
	}
	case 16: {
		for (ssize i = start; i < len; i++)
		{
			char c = Base_ToUpper(str[i]);
			if (!Base_IsHexDigit(c))
			{
				if (endOffset)
				{
					*endOffset = i;
				}
				goto Done;
			}

			s64 digit = Base_IsDecDigit(c) ? c - '0' : c - 'A' + 0xA;
			value = value * 16 + digit;
		}
		break;
	}
	case 2: {
		for (ssize i = start; i < len; i++)
		{
			if (!Base_IsBinDigit(str[i]))
			{
				if (endOffset)
				{
					*endOffset = i;
				}
				goto Done;
			}

			value |= (str[i] - '0') << (len - i - 1);
		}
		break;
	}
	}

	if (endOffset)
	{
		*endOffset = len;
	}
Done:
	return value * sign;
}

BASEAPI f64 Base_ParseFloat(cstr str, ssize* endOffset)
{
	if (!str)
	{
		return 0.0f;
	}

	bool negative = str[0] == '-';
	f64 sign = negative ? -1.0f : 1.0f;
	usize start = (str[0] == '-' || str[0] == '+') ? 1 : 0;

	f64 fractional = 0;
	f64 integral = 0;

	// dubious infinity/nan check, can be skipped cause of -ffast-math/equivalent
	// if (Base_ToLower(str[start]) == 'i')
	//{
	//	return INFINITY * sign;
	//}
	// if (Base_ToLower(str[start]) == 'n')
	//{
	//	return NAN; // NaN's sign isn't specified
	//}

	ssize decimal = Base_StrFind(str, '.');
	ssize len = Base_StrLength(str);
	ssize integralLen = decimal < 0 ? decimal - start : len;

	if (integralLen > 0)
	{
		for (ssize i = start; i < integralLen; i++)
		{
			char c = str[i];
			if (!Base_IsDecDigit(c))
			{
				if (endOffset)
				{
					*endOffset = i;
				}
				goto Done;
			}

			f64 digit = c - '0';
			integral = integral * 10 + digit;
		}
	}

	if (decimal)
	{
		f64 scale = 0.1f;
		for (ssize i = start + integralLen + 1; i < len; i++)
		{
			char c = str[i];
			if (!Base_IsDecDigit(c))
			{
				if (endOffset)
				{
					*endOffset = i;
				}
				goto Done;
			}

			f64 digit = c - '0';
			fractional += digit * scale;
			scale *= 0.1f;
		}
	}

	if (endOffset)
	{
		*endOffset = len;
	}
Done:
	return (integral + fractional) * sign;
}
