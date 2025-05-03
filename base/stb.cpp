/// \file stb build file
/// \copyright 2025 Randomcode Developers

#include "base/base.h"
#include "base/basicstr.h"
#include "base/types.h"

// #define STB_DS_IMPLEMENTATION
// #include "stb.h"

// TODO: this is probably bad, should make LTO actually work
OPTIMIZE_OFF
#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#if defined _MSC_VER && _MSC_VER >= 1900
#define STBSP__ASAN ATTRIBUTE(safebuffers)
#endif
#include "stb/stb_sprintf.h"
OPTIMIZE_ON

BASEAPI u32 Base_VStrFormat(dstr dest, ssize destSize, cstr format, va_list args)
{
	va_list newArgs;
	va_copy(newArgs, args);
	s32 count = stbsp_vsnprintf(dest, static_cast<s32>(destSize), format, newArgs);
	va_end(newArgs);

	// Guarantee NUL termination
	if (dest && destSize > 0)
	{
		dest[destSize - 1] = '\0';
	}

	return static_cast<u32>(count);
}

BASEAPI u32 Base_StrFormat(dstr dest, ssize destSize, cstr format, ...)
{
	va_list args;
	va_start(args, format);
	u32 count = Base_VStrFormat(dest, destSize, format, args);
	va_end(args);

	return count;
}
