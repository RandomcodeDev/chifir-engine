/// Public engine interface for other components

#pragma once

#include "base/types.h"
#include "iapplication.h"

class IEngine : public IApplication
{
  public:
	virtual ~IEngine() DEFAULT;

	virtual void Setup(const CVector<CString>& args) = 0;
	virtual void GetRequiredSystems(CVector<SystemDependency_t>& dependencies) = 0;
	virtual s32 Run(const CVector<ISystem*>& systems /* TODO: , CCommandLine cmdLine */) = 0;

	enum class EngineState_t : s32
	{
		/// The engine hasn't been started yet
		Uninitialized = 0,

		/// The engine is starting up
		Startup = 50,
		/// The engine is shutting down
		Shutdown,

		/// The engine is running and active
		Running = 100,
		/// The engine is running but not active (i.e. the window is unfocused)
		Inactive,
	};

	/// Get whether the engine is headless
	virtual bool Headless() = 0;

	/// Get the engine's current state
	virtual EngineState_t GetState() = 0;

	/// Tell the engine to quit
	virtual void Quit() = 0;
};
