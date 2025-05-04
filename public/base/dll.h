/// \file Definitions for importing/exporting functions from shared libraries
/// \copyright 2025 Randomcode Developers

#pragma once

#include "compiler.h"

#ifdef CH_WIN32
#define DLLEXPORT ATTRIBUTE(dllexport)
#define DLLIMPORT ATTRIBUTE(dllimport)
#elif defined CH_LINUX || defined CH_NX
#define DLLEXPORT ATTRIBUTE(visibility("default"))
#define DLLIMPORT
#else
#define DLLEXPORT
#define DLLIMPORT
#endif
