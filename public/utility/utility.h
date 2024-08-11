// Utility stuff, implemented on top of Base

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

class IApplication;
class ILibrary;
class ISystem;

// Call Base_Quit and also log the message
extern UTILAPI NORETURN void Util_Fatal(cstr message, ...);
