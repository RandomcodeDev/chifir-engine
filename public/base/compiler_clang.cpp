// Clang stuff

#include "base.h"
#include "types.h"

extern "C"
{
	// Presumably emitted for things like a pure virtual call
	void __std_terminate()
	{
		Base_QuitSafe(1, "Invalid behaviour");
	}
}
