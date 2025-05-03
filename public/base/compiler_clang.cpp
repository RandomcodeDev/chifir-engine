/// \file Clang freestanding support
/// \copyright 2025 Randomcode Developers

#include "base.h"
#include "types.h"

extern "C"
{
	/// Presumably emitted for things like a pure virtual call
	NORETURN void __std_terminate()
	{
		Base_AbortSafe(ABORT_RELEVANT_ERROR, "Invalid behaviour");
	}

	/// on the Switch's version of Clang, this is emitted instead of __std_terminate
	void __cxa_pure_virtual()
	{
		Base_AbortSafe(ABORT_RELEVANT_ERROR, "Pure virtual call");
	}

	typedef long long __m128i_clang __attribute__((__vector_size__(16), __aligned__(16)));
	typedef char __v16qi __attribute__((__vector_size__(16)));

#ifndef CH_SWITCH
	void __cxa_atexit()
	{
	}
#endif
}
