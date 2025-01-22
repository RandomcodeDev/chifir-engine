/// Dynamic loading API

#pragma once

#include "platform.h"
#include "types.h"

class ILibrary
{
  public:
	virtual ~ILibrary() DEFAULT;

	/// Get a symbol as a particular type of pointer
	template <typename T> T GetSymbol(cstr name)
	{
		return reinterpret_cast<T>(reinterpret_cast<uptr>(GetSymbol(name)));
	}

	/// Get a symbol
	virtual void* GetSymbol(cstr name) = 0;

	/// Get a library's name
	virtual cstr GetName() = 0;
};

/// Load a library (do not put an extension on the path)
BASEAPI ILibrary* Base_LoadLibrary(cstr name);
