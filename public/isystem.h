// Defines the interface for a "system"

#pragma once

#include "base/loader.h"
#include "base/types.h"
#include "utility/utility.h"

// Describes a system an application wants loaded for it
struct SystemDependency_t
{
	const char* name;
	u32 minimumVersion;
	bool requireExactVersion;
};

// Represents a module of the engine that implements some functionality, such as rendering or windowing or input
class ISystem
{
  public:
	~ISystem() DEFAULT;

	// Initialize the system
	virtual bool Initialize() = 0;

	// Shut down the system
	virtual void Shutdown() = 0;

	// Get the version of this system
	virtual u32 GetVersion() = 0;
};

// Exported by DLLs that implement a system
typedef ISystem* (*CreateInterface)();

// Get a system from a library
UTILAPI ISystem* GetSystem(ILibrary* library);
