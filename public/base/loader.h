/// \file Dynamic loading API
/// \copyright 2025 Randomcode Developers

#pragma once

#include "platform.h"
#include "types.h"

class ILibrary
{
  public:
	virtual ~ILibrary() DEFAULT;

	/// Unload the library
	virtual void Unload() = 0;

	/// Get a symbol as a particular type of pointer
	template <typename T> T GetSymbol(cstr name)
	{
		return reinterpret_cast<T>(reinterpret_cast<uptr>(GetSymbol(name)));
	}

	/// Get a symbol
	virtual void* GetSymbol(cstr name) = 0;

	/// Get a library's name
	virtual cstr GetName() = 0;

	/// Get the library's base address
	virtual uptr GetBase() = 0;
};

// Get a known function
#define GET_SYMBOL(lib, func) ((lib)->GetSymbol<decltype(func)*>(#func))

struct LibDependency_t
{
	cstr name;
	bool required;
};

/// Load a library (do not put an extension on the path)
extern BASEAPI ILibrary* Base_LoadLibrary(cstr name);

/// Load an engine library (just put the base name)
extern BASEAPI ILibrary* Base_LoadEngineLibrary(cstr name);

/// Try loading an engine library, and fall back to the system one
extern BASEAPI ILibrary* Base_LoadEngineOrSystemLibrary(cstr name);
