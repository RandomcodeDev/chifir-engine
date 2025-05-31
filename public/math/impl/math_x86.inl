/// \file x86 math implementation stuff
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

FORCEINLINE f32 ExtractLowerF32(v128 v)
{
#ifdef CH_KNOWN_SIMD256
    return _mm_cvtss_f32(v);
#else
	f32 ret;
    _mm_store_ps(&ret, v);
    return ret;
#endif
}
