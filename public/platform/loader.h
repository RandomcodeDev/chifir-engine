// Dynamic loading API

#pragma once

#include "base/types.h"
#include "platform.h"

class ILibrary
{
	~ILibrary() = default;

	// Get a function
	template <typename T>
	T GetFunction(cstr name)
	{
		return reinterpret_cast<T>(static_cast<uptr>(GetSymbol(name)));
	}

	// Get a symbol
	virtual void* GetSymbol(cstr name) = 0;
};

// Load a library
extern PLATAPI ILibrary* Plat_LoadLibrary(cstr name);
