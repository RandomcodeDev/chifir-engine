// Basic string functions

#pragma once

#include "base.h"
#include "compiler.h"
#include "types.h"

// String length
BASEAPI ssize Base_StrLength(cstr str, ssize maxSize = SSIZE_MAX);

// Compare two strings
BASEAPI s32 Base_StrCompare(cstr RESTRICT a, cstr RESTRICT b, ssize maxCount = SSIZE_MAX, bool caseSensitive = true);

// Format a string
BASEAPI u32 Base_VStrFormat(dstr dest, u32 destSize, cstr format, va_list args);
BASEAPI u32 Base_StrFormat(dstr dest, u32 destSize, cstr format, ...);

// Format a string, and allocate the buffer for it
BASEAPI dstr Base_VStrFormat(cstr format, va_list args);
BASEAPI dstr Base_StrFormat(cstr format, ...);

// Copy a string
BASEAPI dstr Base_StrCopy(dstr RESTRICT dest, cstr RESTRICT src, ssize maxCount);

// Duplicate a string
BASEAPI dstr Base_StrClone(cstr str, ssize maxSize = SSIZE_MAX);
