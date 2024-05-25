/// @file ISystem.h
/// @brief This file defines the interface for systems
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class ISystem
{
  public:
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual u32 GetVersion() = 0;
};

struct SystemDependency
{
	std::string name;
	u32 minimumVersion;
	bool requireExactVersion;
	bool optional;
	ISystem** system;
};
