#include "base.h"
#include "base/base.h"
#include "base/compiler.h"
#include "base/types.h"

BASEAPI NORETURN void Base_Quit(s32 error, cstr msg, ...)
{
	// TODO: formatting, when I implement snprintf or use stb to do that
	Base_QuitImpl(error, msg);
}
