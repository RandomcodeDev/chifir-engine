#include "base/compiler.h"
#include "base/types.h"
#include "misc.h"

BASEAPI NORETURN void Base_Quit(u32 error, cstr msg, ...)
{
	// TODO: formatting, when I implement snprintf or use stb to do that
	Base_QuitImpl(error, msg);
}
