// Internal definitions for the launcher

#pragma once

#include "base/types.h"

#ifdef CH_XBOX360
#define LAUNCHERAPI DLLEXPORT
#else
#define LAUNCHERAPI
#endif

// Cross-platform entry point
extern "C" LAUNCHERAPI s32 LauncherMain();
