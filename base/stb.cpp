#include "base/base.h"
#include "base/basicstr.h"
#include "base/types.h"

#define STBDS_NO_SHORT_NAMES
#define STBDS_REALLOC Base_Realloc
#define STBDS_FREE Base_Free
#include "stb/stb_ds.h"

#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#include "stb/stb_sprintf.h"

BASEAPI u32 Base_VStrPrint(dstr dest, u32 destSize, cstr format, va_list args)
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

BASEAPI u32 Base_StrPrint(dstr dest, u32 destSize, cstr format, ...)
{
	va_list args;
	va_start(args, format);
	u32 count = Base_VStrPrint(dest, destSize, format, args);
	va_end(args);

	return count;
}
