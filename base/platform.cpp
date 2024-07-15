#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/types.h"

BASEAPI NORETURN void Base_Quit(s32 error, cstr msg, ...)
{
	// Used if Base_VFormat fails;
	char buffer[1024];


	va_list args;
	va_start(args, msg);
	dstr formattedMsg = Base_VFormat(msg, args);
	if (!formattedMsg)
	{
		formattedMsg = buffer;
		Base_VStrPrint(buffer, ARRAY_SIZE(buffer), msg, args);
	}
	va_end(args);
	Base_QuitSafe(error, formattedMsg);
}
