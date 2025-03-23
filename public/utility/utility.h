// Utility stuff, implemented on top of Base
// Copyright 2025 Randomcode Developers

#pragma once

#include "base/base.h"
#include "base/basicstr.h"
#include "base/dll.h"

#ifdef CH_STATIC
#define UTILAPI
#else
#ifdef IN_UTIL
#define UTILAPI DLLEXPORT
#else
#define UTILAPI DLLIMPORT
#endif
#endif
