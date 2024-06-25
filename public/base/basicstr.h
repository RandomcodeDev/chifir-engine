// Basic string functions

#pragma once

#include "base.h"
#include "compiler.h"
#include "types.h"

// String length
extern BASEAPI usize Base_StrLen(cstr str, usize maxSize = SIZE_MAX);
