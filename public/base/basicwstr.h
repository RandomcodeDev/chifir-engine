// Basic functions for wide strings

#pragma once

#include "base.h"
#include "compiler.h"
#include "types.h"

// Wide string length
BASEAPI usize Base_WStrLen(cwstr str);

// Compare two wide strings
BASEAPI s32 Base_WStrCmp(cwstr RESTRICT a, cwstr RESTRICT b, bool caseSensitive = true, usize maxCount = SIZE_MAX);
