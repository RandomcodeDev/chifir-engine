/// @file ISharedLibrary.h
/// @brief This file defines the interface for shared libraries
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class ISharedLibrary
{
  public:
	virtual ~ISharedLibrary() = 0;
	virtual std::function<void()> GetFunction(const std::string& name) = 0;
	virtual void* GetSymbol(const std::string& name) = 0;

	virtual const std::string& GetName();
	virtual uint64_t GetHandle();
};
