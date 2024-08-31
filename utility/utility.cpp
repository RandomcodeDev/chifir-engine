// Miscellaneous utility functions

#include "utility/utility.h"
#include "base/loader.h"
#include "base/log.h"
#include "isystem.h"

UTILAPI NORETURN void Util_Fatal(cstr message, ...)
{
	va_list args;

	va_start(args, message);
	cstr formatted = Base_VStrFormat(message, args);
	va_end(args);
	if (!formatted)
	{
		formatted = message;
	}

	Log_FatalError("%s", formatted);
	Base_QuitSafe(1, formatted);
}
