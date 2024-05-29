/// @file IPlatform.h
/// @brief This file defines the interface for abstracting the underlying OS
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class ISharedLibrary;

class IPlatform
{
  public:
	/// @brief Set up desirable state for the platform
	virtual void Initialize() = 0;

	/// @brief Clean up anything
	virtual void Shutdown() = 0;

	/// @brief Get a description of the OS
	virtual const std::string& DescribeOs() = 0;

	/// @brief Get a description of the hardware (CPU, RAM, GPU, etc)
	virtual const std::string& DescribeHardware() = 0;

	/// @brief Get the path to the directory where data can be saved (AppData, ~/.local/share, etc)
	virtual const std::string& GetUserDataPath() = 0;

	/// @brief Kill the program
	[[noreturn]] virtual void Quit(
		const std::string& message, bool useLastError = true, const std::string& file = "", const std::string& function = "", int line = 0) = 0;

	/// @brief Load a shared library. This should be reset before you call Shutdown.
	virtual std::shared_ptr<ISharedLibrary> LoadLibrary(
		const std::string& name, const std::vector<std::string>& paths = std::vector<std::string>()) = 0;

	/// @brief Create a directory
	virtual bool CreateDirectory(const std::string& path) = 0;
};

extern FRAMEWORK_API IPlatform* GetPlatform();
