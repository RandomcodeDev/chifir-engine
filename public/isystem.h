// Defines the interface for a "system"

#pragma once

#include "base/types.h"

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
	~ISystem()
	{
	}

	// Initialize the system
	bool Initialize();

	// Shut down the system
	void Shutdown();

	// Get the version of this system
	u32 GetVersion();
};

// Exported by DLLs that implement a system
typedef ISystem* (*CreateInterface)();

