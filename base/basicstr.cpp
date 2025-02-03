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

BASEAPI s32 Base_StrCompare(cstr RESTRICT a, cstr RESTRICT b, ssize maxCount, bool caseSensitive)
{
	// TODO: case insensitive string compare
	ASSERT_MSG(caseSensitive != false, "Case-insensitive string compare not yet implemented");
	ssize size = Min(Base_StrLength(a), maxCount);
	return Base_MemCompare(a, b, size);
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
