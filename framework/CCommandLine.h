/// @file CCommandLine.h
/// @brief This file declares a class for manipulating command lines
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class FRAMEWORK_API CCommandLine
{
  public:
	static constexpr auto OPTION_PREFIXES = "-/";

	CCommandLine(int argc, char* argv[]);
	CCommandLine(const std::string& cmdLine);
	CCommandLine(const std::vector<std::string>& args);

	const std::string& GetName() const
	{
		return m_name;
	}

	bool HasArg(const std::string& name) const
	{
		return m_args.find(name) != m_args.end();
	}

	const std::string& GetArg(const std::string& name) const;

	const std::map<std::string, std::string>& GetArgs() const
	{
		return m_args;
	}

  private:
	std::string m_name;
	std::map<std::string, std::string> m_args;

	void Parse(const std::vector<std::string>& args);
};
