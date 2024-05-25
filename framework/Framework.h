/// @file Framework.h
/// @brief This file has definitions used throughout everything.
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include <cctype>
#include <cinttypes>
#include <functional>
#include <limits>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#ifdef KR_PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef KR_USE_MIMALLOC
#include "mimalloc.h"
#endif

#ifdef KR_PLATFORM_WINDOWS
#ifdef KR_FRAMEWORK
#define FRAMEWORK_API __declspec(dllexport)
#else
#define FRAMEWORK_API __declspec(dllimport)
#endif
#else
#ifdef KR_FRAMEWORK
#define FRAMEWORK_API __attribute__((visibility("default")))
#else
#define FRAMEWORK_API
#endif
#endif
