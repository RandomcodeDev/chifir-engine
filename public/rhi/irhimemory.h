/// \file RHI memory stuff
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

/// Where memory is located
enum class RhiMemoryLocation
{
	Host,
	Device,
	Shared
};

