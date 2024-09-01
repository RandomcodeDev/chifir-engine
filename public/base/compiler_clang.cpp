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

//#ifdef CH_X86
//	s32 _mm_cmpestrc(__m128i a, s32 aLength, __m128i b, s32 bLength, const s32 mode)
//		__attribute__((alias("__builtin_ia32_pcmpestric128")));
//#endif
}
