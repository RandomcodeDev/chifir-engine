/// @file IApplication.h
/// @brief This file defines the interface for applications
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class CCommandLine;
class ISystem;
struct SystemDependency;

class IApplication
{
  public:
	/// @brief The destructor probably doesn't have to do much
	virtual ~IApplication() = default;

	/// @brief Organization of the game (e.g. "Randomcode Developers")
	virtual const char* GetOrganization() const = 0;

	/// @brief Name of the game (e.g. "False King")
	virtual const char* GetName() const = 0;

	/// @brief Secondary name (e.g. "false_king")
	virtual const char* GetIdName() const = 0;

	/// @brief Get the systems this application depends on
	virtual std::vector<SystemDependency> GetSystems() const = 0;

	/// @brief Run the application
	virtual void Run(const CCommandLine& cmdLine, const std::unordered_map<std::string, ISystem*>& systems) = 0;

};
