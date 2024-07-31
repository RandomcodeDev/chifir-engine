#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/dll.h"
#include "base/types.h"

BASEAPI ssize Base_StrLength(cstr str, ssize maxSize)
{
	ssize size;
	for (size = 0; size < maxSize && str[size] != '\0'; size++)
		;

	return size;
}

BASEAPI s32 Base_StrCompare(cstr RESTRICT a, cstr RESTRICT b, ssize maxCount, bool caseSensitive)
{
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
	Base_MemCopy(dest, src, size);
	dest[size] = '\0';
	return dest;
}

BASEAPI dstr Base_StrClone(cstr str, ssize maxSize)
{
	ssize size = Min(Base_StrLength(str), maxSize) + 1;
	dstr buffer = Base_Alloc<char>(size);
	if (!buffer)
	{
		return nullptr;
	}

	return Base_StrCopy(buffer, str, size);
}
