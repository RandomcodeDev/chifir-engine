/// @file ISystem.h
/// @brief This file defines the interface for systems
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

class ISystem
{
  public:
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual uint32_t GetVersion() = 0;
};

struct SystemDependency
{
	std::string name;
	uint32_t minimumVersion;
	bool requireExactVersion;
	bool optional;
	ISystem** system;
};
