/// @file IApplication.h
/// @brief This file defines the interface for applications
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class CCommandLine;
struct SystemDependency;

class IApplication
{
  public:
	/// @brief Organization of the game (e.g. "Randomcode Developers")
	virtual const std::string& GetOrganization() = 0;

	/// @brief Name of the game (e.g. "False King")
	virtual const std::string& GetName() = 0;

	/// @brief Secondary name (e.g. "false_king")
	virtual const std::string& GetIdName() = 0;

	/// @brief Get the systems this application depends on
	virtual std::vector<SystemDependency> GetSystems() = 0;

	/// @brief Run the application
	virtual void Run(CCommandLine* cmdLine) = 0;

  protected:
	CCommandLine* m_cmdLine;
};
