/// @file IPlatform.h
/// @brief This file defines the interface for abstracting the underlying OS
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"
#include "ISharedLibrary.h"

class IPlatform
{
  public:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual 
};
