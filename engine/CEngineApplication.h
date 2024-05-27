/// @file CEngineApplication.h
/// @brief This file defines the engine's application class
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"
#include "framework/IApplication.h"

class CEngineApplication : public IApplication
{
  public:
	std::vector<SystemDependency> GetSystems() const;

	void Run(CCommandLine* cmdLine);

	const char* GetOrganization() const
	{
		return ORGANIZATION;
	}

	const char* GetName() const
	{
		return NAME;
	}

	const char* GetIdName() const
	{
		return ID_NAME;
	}

  private:
	static constexpr const char* ORGANIZATION = "Randomcode Developers";
	static constexpr const char* NAME = "False King";
	static constexpr const char* ID_NAME = "false_king";
};
