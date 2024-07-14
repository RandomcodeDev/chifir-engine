#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/dll.h"
#include "base/types.h"

BASEAPI usize Base_StrLength(cstr str, usize maxSize)
{
	usize size;
	for (size = 0; size < maxSize && str[size] != '\0'; size++)
		;

	return size;
}

BASEAPI dstr Base_VFormat(cstr format, va_list args)
{
	va_list newArgs;

	va_copy(newArgs, args);
	// Size of string plus nul terminator
	u32 size = Base_VStrPrint(nullptr, 0, format, newArgs) + 1;
	dstr buf = Base_Alloc<char>(size);
	if (!buf)
	{
		va_end(args);
		return nullptr;
	}
	Base_VStrPrint(buf, size, format, newArgs);
	va_end(args);

	return buf;
}

BASEAPI dstr Base_Format(cstr format, ...)
{
	va_list args;

	va_start(args, format);
	dstr buf = Base_VFormat(format, args);
	va_end(args);

	return buf;
}

BASEAPI dstr Base_StrCopy(dstr RESTRICT dest, cstr RESTRICT src, usize maxCount)
{
	usize size = Min(Base_StrLength(src), maxCount);
	Base_MemCopy(dest, src, size);
	dest[size] = '\0';
	return dest;
}

BASEAPI dstr Base_StrClone(cstr str, usize maxSize)
{
	usize size = Min(Base_StrLength(str), maxSize) + 1;
	dstr buffer = Base_Alloc<char>(size);
	if (!buffer)
	{
		return buffer;
	}

	return Base_StrCopy(buffer, str, size);
}
