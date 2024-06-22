// Compiler-specific defines

#pragma once

#ifdef _MSC_VER
#define ATTRIBUTE(x) __declspec(x)
#else
#define ATTRIBUTE(x) __attribute__((x))
#endif

