/// @file CCommandLine.h
/// @brief This file declares a class for manipulating command lines
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class FRAMEWORK_API CCommandLine
{
  public:
	CCommandLine(int argc, char* argv[]);
	CCommandLine(const std::string& cmdLine);
	CCommandLine(const std::vector<std::string>& args);

	const std::string& GetName()
	{
		return m_name;
	}
	const std::vector<std::string>& GetArgs()
	{
		return m_args;
	}

  private:
	std::string m_name;
	std::vector<std::string> m_args;
};
