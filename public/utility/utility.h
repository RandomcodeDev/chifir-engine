// Utility stuff, implemented on top of Base

#pragma once

#include "base/base.h"
#include "base/basicstr.h"
#include "base/dll.h"

#ifdef IN_UTIL
#define UTILAPI DLLEXPORT
#else
#define UTILAPI DLLIMPORT
#endif

class ILibrary;
class ISystem;

// Call Base_Quit and also log the message
UTILAPI NORETURN void Util_Fatal(cstr message, ...);

// Get a system from a library
UTILAPI ISystem* Util_GetSystem(ILibrary* library, u32 minVersion = 0);
