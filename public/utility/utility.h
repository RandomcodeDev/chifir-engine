// Utility stuff, implemented on top of Base

#pragma once

#include "base/base.h"
#include "base/dll.h"

#ifdef IN_UTIL
#define UTILAPI DLLEXPORT
#else
#define UTILAPI DLLIMPORT
#endif

class ILibrary;
class ISystem;

// Get a system from a library
UTILAPI ISystem* GetSystem(ILibrary* library, u32 minVersion = 0);
