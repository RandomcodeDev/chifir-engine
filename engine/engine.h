// Engine implementation class

#pragma once

#include "base/filesystem.h"
#include "engine/engine.h"
#include "iapplication.h"

class IVideoSystem;

class CEngine : public IEngine
{
  public:
	CEngine() : m_state(EngineStateUninitialized), m_videoSystem(nullptr)
	{
	}
	~CEngine() DEFAULT;

	void GetRequiredSystems(CVector<SystemDependency_t>& dependencies);
	s32 Run(const CVector<ISystem*>& systems /* TODO: , CCommandLine cmdLine */);

	EngineState_t GetState()
	{
		return m_state;
	}

	void Quit()
	{
		m_state = EngineStateShutdown;
	}

  private:
	EngineState_t m_state;
	bool m_inFrame;

	//IFilesystem* m_mainFilesystem;
	IWritableFilesystem* m_saveFilesystem;

	IVideoSystem* m_videoSystem;

	// Initializes the filesystem for save files/other data
	bool InitializeSaveFilesystem();

	// Adds log writers
	void AddLogWriters();

	// Initializes the engine's systems in the right order
	bool InitializeSystems();

	// Pre frame stuff
	void PreFrame();

	// Mid frame stuff
	void Update();

	// Post frame stuff
	void PostFrame();

	// Shuts down the engine's systems in the right order
	void ShutdownSystems();

	// Check if the engine's state should change
	void CheckState();
};
