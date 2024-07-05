// Definitions for importing/exporting functions from shared libraries

#pragma once

#include "compiler.h"

#ifdef CH_WIN32
#define DLLEXPORT ATTRIBUTE(dllexport)
#define DLLIMPORT ATTRIBUTE(dllimport)
#else
#define DLLEXPORT ATTRIBUTE(visibility("default"))
#define DLLIMPORT
#endif
