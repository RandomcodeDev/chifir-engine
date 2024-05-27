/// @file ISharedLibrary.h
/// @brief This file defines the interface for shared libraries
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

class ISharedLibrary
{
  public:
	virtual void* GetSymbol(const std::string& name) const = 0;

	template <class Ret, class... Args>
	std::function<Ret(Args...)> GetFunction(const std::string& name) const
	{
		return static_cast<Ret (*)(Args...)>(GetSymbol(name));
	}

	virtual const std::string& GetName() const = 0;
	virtual u64 GetHandle() const = 0;
};
