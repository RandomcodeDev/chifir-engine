/// @file Framework.h
/// @brief This file has definitions used throughout everything.
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <limits>
#include <memory>

// TODO: implement these myself some day (it can't be that hard, I just don't want to yet)
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shlobj.h>

#undef LoadLibrary
#undef CreateDirectory

#ifndef KR_PLATFORM_XBOX
#include <dbghelp.h>
#endif

#ifdef KR_PLATFORM_GDK
#include <XGameRuntimeInit.h>
#endif

#include "fmt/fmt.h"

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using uptr = uintptr_t;
using sptr = intptr_t;

using usize = uptr;
using ssize = sptr;

#ifdef KR_PLATFORM_WINDOWS
#define KR_EXPORT __declspec(dllexport)
#define KR_IMPORT __declspec(dllimport)
#else
#define KR_EXPORT __attribute__((visibility("default")))
#define KR_IMPORT
#endif

#ifdef KR_FRAMEWORK
#define FRAMEWORK_API KR_EXPORT
#else
#define FRAMEWORK_API KR_IMPORT
#endif
