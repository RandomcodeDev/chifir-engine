// Public engine interface for other components

#pragma once

#include "iapplication.h"
#include "base/types.h"

class IEngine : public IApplication
{
  public:
	virtual ~IEngine() DEFAULT;

	virtual void GetRequiredSystems(CVector<SystemDependency_t>& dependencies) = 0;
	virtual s32 Run(const CVector<ISystem*>& systems /* TODO: , CCommandLine cmdLine */) = 0;

	enum EngineState_t
	{
		EngineStateUninitialized = 0, // The engine hasn't been started yet

		EngineStateStartup = 50, // The engine is starting up
		EngineStateShutdown, // The engine is shutting down

		EngineStateRunning = 100, // The engine is running and active
		EngineStateInactive, // The engine is running but not active (i.e. the window is unfocused)
	};

	// Get the engine's current state
	virtual EngineState_t GetState() = 0;

	// Tell the engine to quit
	virtual void Quit() = 0;
};
