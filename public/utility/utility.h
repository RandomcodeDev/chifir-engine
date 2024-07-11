// Utility stuff, implemented on top of Base

#pragma once

#include "base/base.h"
#include "base/dll.h"

#ifdef IN_UTIL
#define UTILAPI DLLEXPORT
#else
#define UTILAPI DLLIMPORT
#endif

