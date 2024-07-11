// Dynamic loading API

#pragma once

#include "types.h"
#include "platform.h"

class ILibrary
{
  public:
	~ILibrary() DEFAULT;

	// Get a symbol as a particular type of pointer
	template <typename T> T GetSymbol(cstr name)
	{
		return reinterpret_cast<T>(reinterpret_cast<uptr>(GetSymbol(name)));
	}

	// Get a symbol
	virtual void* GetSymbol(cstr name) = 0;
};

// Load a library
BASEAPI ILibrary* Base_LoadLibrary(cstr name);
