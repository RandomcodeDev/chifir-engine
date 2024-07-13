// Basic string functions

#pragma once

#include "base.h"
#include "compiler.h"
#include "types.h"

// String length
BASEAPI usize Base_StrLength(cstr str, usize maxSize = SIZE_MAX);

// Format a string
BASEAPI u32 Base_VStrPrint(dstr dest, u32 destSize, cstr format, va_list args);
BASEAPI u32 Base_StrPrint(dstr dest, u32 destSize, cstr format, ...);

// Format a string, and allocate the buffer for it
BASEAPI dstr Base_VFormat(cstr format, va_list args);
BASEAPI dstr Base_Format(cstr format, ...);
