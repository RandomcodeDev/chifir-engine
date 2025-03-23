/// \file Internal definitions for the launcher
/// \copyright Randomcode Developers

#pragma once

#include "base/dll.h"
#include "base/types.h"

#ifdef CH_STATIC
#define LAUNCHERAPI
#else
#define LAUNCHERAPI DLLEXPORT
#endif

/// Cross-platform entry point
extern "C" LAUNCHERAPI s32 LauncherMain();
