/// \file RHI command list interfaces
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/types.h"

class IRhiCommandList
{
  public:
	virtual ~IRhiCommandList() = default;
};

enum class RhiCommandListFlags_t
{
};

class IRhiFence
{
};
