/// Public engine interface for other components

#pragma once

#include "iapplication.h"
#include "base/types.h"

class IEngine : public IApplication
{
  public:
	virtual ~IEngine() DEFAULT;

	virtual void GetRequiredSystems(CVector<SystemDependency_t>& dependencies) = 0;
	virtual s32 Run(const CVector<ISystem*>& systems /* TODO: , CCommandLine cmdLine */) = 0;

	enum class EngineState_t : s32
	{
		Uninitialized = 0, /// The engine hasn't been started yet

		Startup = 50, /// The engine is starting up
		Shutdown, /// The engine is shutting down

		Running = 100, /// The engine is running and active
		Inactive, /// The engine is running but not active (i.e. the window is unfocused)
	};

	/// Get the engine's current state
	virtual EngineState_t GetState() = 0;

	/// Tell the engine to quit
	virtual void Quit() = 0;
};
