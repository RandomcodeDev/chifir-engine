// Clang stuff

#include "base.h"
#include "types.h"

extern "C"
{
	// Presumably emitted for things like a pure virtual call
	void __std_terminate()
	{
		Base_AbortSafe(1, "Invalid behaviour");
	}

	// on Switch this is emitted instead of __std_terminate
	void __cxa_pure_virtual()
	{
		Base_AbortSafe(1, "Pure virtual call");
	}

//#ifdef CH_X86
//	s32 _mm_cmpestrc(__m128i a, s32 aLength, __m128i b, s32 bLength, const s32 mode)
//		__attribute__((alias("__builtin_ia32_pcmpestric128")));
//#endif
}
