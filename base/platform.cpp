/// \file Platform-independent higher level platform functions
/// \copyright 2025 Randomcode Developers

#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/types.h"

BASEAPI NORETURN void Base_Abort(s32 error, cstr msg, ...)
{
	// Used if Base_VFormat fails;
	char buffer[1024];

	va_list args;
	va_start(args, msg);
	dstr formattedMsg = Base_VStrFormat(msg, args);
	if (!formattedMsg)
	{
		formattedMsg = buffer;
		Base_VStrFormat(buffer, ArraySize(buffer), msg, args);
	}
	va_end(args);
	Base_AbortSafe(error, formattedMsg);
}
