/// Internal definitions for the launcher

#pragma once

#include "base/types.h"

#define LAUNCHERAPI DLLEXPORT

/// Cross-platform entry point
extern "C" LAUNCHERAPI s32 LauncherMain();
