/// \file RHI memory buffers
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

enum class RhiMemoryLocation
{
    Host,
    Device,
    Shared
};

enum class RhiMemoryUsage
{

};

class IRhiBuffer;
