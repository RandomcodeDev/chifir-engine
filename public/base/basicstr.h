// Basic string functions

#pragma once

#include "base.h"
#include "compiler.h"
#include "types.h"

// String length
BASEAPI usize Base_StrLen(cstr str, usize maxSize = SIZE_MAX);

// Format a string
BASEAPI u32 Base_VStrPrint(dstr dest, u32 destSize, cstr format, va_list args);
BASEAPI u32 Base_StrPrint(dstr dest, u32 destSize, cstr format, ...);
