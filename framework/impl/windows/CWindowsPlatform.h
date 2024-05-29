/// @file CWindowsPlatform.h
/// @brief This file defines the Windows implementation of IPlatform
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"
#include "framework/IPlatform.h"

class CWindowsPlatform : public IPlatform
{
  public:
	void Initialize();
	void Shutdown();

	const std::string& DescribeOs();

	const std::string& DescribeHardware();

	const std::string& GetUserDataPath();

	[[noreturn]] void Quit(const std::string& message, bool useLastError, const std::string& file, const std::string& function, int line);

	std::shared_ptr<ISharedLibrary> LoadLibrary(const std::string& name, const std::vector<std::string>& paths);

	bool CreateDirectory(const std::string& name);
};
