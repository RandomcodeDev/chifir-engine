/// \file System interface
/// \copyright Randomcode Developers

#pragma once

#include "base/loader.h"
#include "base/types.h"
#include "utility/utility.h"

/// Describes a system an application wants loaded for it
struct SystemDependency_t
{
	const char* name;         /// The name of the DLL where the system can be found
	u32 minimumVersion;       /// The minimum version of the system
	bool required;            /// Whether the system is required
	bool requireExactVersion; /// Whether the version of the system can exceed the minimum
};

/// Represents a module of the engine that implements some functionality, such as rendering or windowing or input
class ISystem
{
  public:
	virtual ~ISystem() DEFAULT;

	/// Initialize the system
	virtual bool Initialize() = 0;

	/// Shut down the system
	virtual void Shutdown() = 0;

	/// Get the name of this system
	virtual cstr GetName() const = 0;

	/// Get the version of this system
	virtual u32 GetVersion() const = 0;
};

/// Exported by DLLs that implement a system as "CreateInterface"
typedef ISystem* (*CreateSystemInterface_t)();
