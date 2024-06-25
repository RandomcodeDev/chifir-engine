// Includes definitions for making xxHash work

#pragma once

#include "base/base.h"

#define XXH_INLINE_ALL
#define XXH_NO_STREAMING
#define memcpy(dest, src, size) (Base_MemCpy(dest, src, size), dest)
#include "xxhash.h"
#undef memcpy

