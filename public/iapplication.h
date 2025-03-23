/// \file Launcher application interface
/// \copyright Randomcode Developers

/// Application interface, for the launcher. An application is a DLL that the launcher loads, and tells it what systems
/// it wants, and the launcher loads them and checks that they're the right version.

#pragma once

#include "base/compiler.h"
#include "base/loader.h"
#include "base/vector.h"
#include "isystem.h"

class IApplication
{
  public:
	virtual ~IApplication() DEFAULT;

	/// Prepare the application to run
	virtual void Setup(const CVector<CString>& args) = 0;

	/// Get the application's required systems and libraries
	virtual void GetDependencies(CVector<SystemDependency_t>& systems, CVector<LibDependency_t>& libs) = 0;

	/// Run the application
	/// systems is in the same order as the list from GetRequiredSystems, with nullptrs for optional systems that couldn't be
	/// loaded
	virtual s32 Run(const CVector<ISystem*>& systems) = 0;
};

/// Exported by DLLs that contain an application as "CreateInterface"
typedef IApplication* (*CreateApplicationInterface_t)();
