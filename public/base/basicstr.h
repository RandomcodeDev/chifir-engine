// Basic string functions

#pragma once

#include "base.h"
#include "compiler.h"
#include "types.h"

// String length
extern BASEAPI ssize Base_StrLength(cstr str, ssize maxSize = SSIZE_MAX);

// Compare two strings
extern BASEAPI s32 Base_StrCompare(cstr RESTRICT a, cstr RESTRICT b, ssize maxCount = SSIZE_MAX, bool caseSensitive = true);

// Format a string
extern BASEAPI u32 Base_VStrFormat(dstr dest, u32 destSize, cstr format, va_list args);
extern BASEAPI u32 Base_StrFormat(dstr dest, u32 destSize, cstr format, ...);

// Format a string, and allocate the buffer for it
extern BASEAPI dstr Base_VStrFormat(cstr format, va_list args);
extern BASEAPI dstr Base_StrFormat(cstr format, ...);

// Copy a string
extern BASEAPI dstr Base_StrCopy(dstr RESTRICT dest, cstr RESTRICT src, ssize maxCount);

// Duplicate a string
extern BASEAPI dstr Base_StrClone(cstr str, ssize maxSize = SSIZE_MAX);
