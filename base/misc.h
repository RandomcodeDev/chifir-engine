// Miscellaneous functions

#pragma once

#include "base/compiler.h"
#include "base/platform.h"
#include "base/types.h"

// So individual implementations don't have to handle formatting the message
extern NORETURN void Base_QuitImpl(u32 code, cstr msg);
