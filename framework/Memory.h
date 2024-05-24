/// @file Memory.h
/// @brief This file declares memory-related functions
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

/// @brief Allocate memory
extern FRAMEWORK_API void* AllocateMemory(size_t size);

/// @brief Free memory
extern FRAMEWORK_API void FreeMemory(void* memory);
