/// Application interface, for the launcher. An application is a DLL that the launcher loads, and tells it what systems
/// it wants, and the launcher loads them and checks that they're the right version.

#pragma once

#include "base/compiler.h"
#include "base/vector.h"
#include "isystem.h"

class IApplication
{
  public:
	virtual ~IApplication() DEFAULT;

	/// Get the application's required systems
	virtual void GetRequiredSystems(CVector<SystemDependency_t>& dependencies) = 0;

	/// Run the application
	/// systems is in the same order as the list from GetRequiredSystems, with nullptrs for optional systems that couldn't be
	/// loaded
	/// TODO: command line parsing
	virtual s32 Run(const CVector<ISystem*>& systems /* TODO: , CCommandLine cmdLine */) = 0;
};

/// Exported by DLLs that contain an application as "CreateInterface"
typedef IApplication* (*CreateApplicationInterface_t)();
