/// \file Undefines conflicting Win32 Unicode/ANSI aliases, and defines wrapper macros for weird versions of Windows headers
/// \copyright 2025 Randomcode Developers

#pragma once

#undef CreateDirectory

#ifdef CH_XBOX
#define Nt
#else
#endif
