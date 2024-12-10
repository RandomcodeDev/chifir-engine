// Clang stuff

#include "base.h"
#include "types.h"

extern "C"
{
	// Presumably emitted for things like a pure virtual call
	NORETURN void __std_terminate()
	{
		Base_AbortSafe(1, "Invalid behaviour");
	}

	// on Switch this is emitted instead of __std_terminate
	void __cxa_pure_virtual()
	{
		Base_AbortSafe(1, "Pure virtual call");
	}

#ifndef CH_SWITCH
	void __cxa_atexit()
	{
		
	}
#endif
}
