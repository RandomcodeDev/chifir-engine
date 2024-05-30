/// @file CEngineApplication.h
/// @brief This file defines the engine's application class
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"
#include "framework/IApplication.h"

#include "engine/Engine.h"

class CEngineApplication : public IApplication
{
  public:
	CEngineApplication() = default;
	~CEngineApplication() = default;

	std::vector<SystemDependency> GetSystems() const;

	void Run(const CCommandLine& cmdLine, const std::unordered_map<std::string, ISystem*>& systems);

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

	bool m_running;

	/// @brief The engine's main loop
	void Loop();
};
