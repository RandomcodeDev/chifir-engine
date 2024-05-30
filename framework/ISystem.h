/// @file ISystem.h
/// @brief This file defines the interface for systems
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class ISystem
{
  public:
	// Constructor: register any variables

	virtual ~ISystem() = default;

	/// @brief Initialize the system
	virtual bool Initialize() = 0;

	/// @brief Shut down the system
	virtual void Shutdown() = 0;
};

struct SystemDependency
{
	std::string name;
	u32 minimumVersion;
	bool requireExactVersion;
	bool optional;
};
