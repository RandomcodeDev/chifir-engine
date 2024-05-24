/// @file IApplication.h
/// @brief This file defines the interface for applications
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"
#include "CCommandLine.h"
#include "ISystem.h"

class IApplication
{
  public:
	virtual std::vector<SystemDependency> GetSystems() = 0;

	virtual void Run(CCommandLine* cmdLine) = 0;

  protected:
	CCommandLine* m_cmdLine;
};
