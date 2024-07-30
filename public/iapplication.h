// Application interface, for the launcher. An application is a DLL that the launcher loads, and tells it what systems
// it wants, and the launcher loads them and checks that they're the right version.

#pragma once

#include "base/compiler.h"
#include "base/vector.h"
#include "isystem.h"

class IApplication
{
  public:
	virtual ~IApplication() DEFAULT;

	// Get the application's required systems
	virtual CVector<SystemDependency_t> GetRequiredSystems() = 0;

	// Run the application
	// systems is in the same order as the list from GetRequiredSystems, but any that aren't required and couldn't be loaded will
	// be replaced with nullptr
	// TODO: command line parsing
	virtual s32 Run(CVector<ISystem*> systems /* TODO: , CCommandLine cmdLine */) = 0;
};

// Exported by DLLs that contain an application as "CreateInterface"
typedef IApplication* (*CreateApplicationInterface_t)();
