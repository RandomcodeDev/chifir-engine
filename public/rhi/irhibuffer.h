/// \file RHI memory buffers
/// \copyright 2025 Randomcode Developers

#pragma once

enum class RhiMemoryLocation
{
    Host,
    Device,
    Shared
};

enum class RhiMemoryUsage;
class IRhiBuffer;
