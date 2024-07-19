// To ensure all non-static stb headers work correctly

#pragma once

#include "base/base.h"
#include "base/basicstr.h"

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#define STBDS_NO_SHORT_NAMES
#define STBDS_NO_STDLIB
#define STBDS_REALLOC(ctx, ptr, size)   Base_Realloc((ptr), (size))
#define STBDS_FREE(ctx, ptr)            Base_Free(ptr)
#define STBDS_STRLEN(x)                 Base_StrLength(x)
#define STBDS_STRCMP(a, b)              Base_StrCompare(a, b)
#define STBDS_MEMCMP(a, b, size)        Base_MemCompare(a, b, size)
#define STBDS_MEMMOVE(dest, src, size)  Base_MemCopy(dest, src, size)
#define STBDS_MEMCPY                    STBDS_MEMMOVE
#define STBDS_MEMSET(dest, value, size) Base_MemSet(dest, value, size)
#define STBDS_PUBLICDEF                 BASEAPI
#include "stb/stb_ds.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
