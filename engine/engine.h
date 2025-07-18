/// \file Engine core implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/string.h"
#include "engine/engine.h"
#include "iapplication.h"
#include "rendersystem/irendersystem.h"
#include "rhi/rhi.h"
#include "videosystem/ivideosystem.h"

class IWritableFilesystem;
class IRenderSystem;
class IVideoSystem;

class CEngine: public IEngine
{
  public:
	CEngine();
	~CEngine();

	virtual void Setup(const CVector<CString>& args);
	virtual void GetDependencies(CVector<SystemDependency>& systems, CVector<LibDependency_t>& libs);
	virtual s32 Run(const CVector<ISystem*>& systems /* TODO: , CCommandLine cmdLine */);

	virtual bool Headless()
	{
		return m_headless;
	}

	virtual const DateTime& GetStartTime()
	{
		return m_startTime;
	}

	virtual EngineState GetState()
	{
		return m_state;
	}

	virtual void Quit()
	{
		m_state = EngineState::Shutdown;
	}

  private:
	EngineState m_state;
	DateTime m_startTime;
	bool m_headless;
	cstr m_rhiBackendName;
	bool m_inFrame;

	// IFilesystem* m_mainFilesystem;
	IWritableFilesystem* m_dataFilesystem;

	IRenderSystem* m_renderSystem;
	IVideoSystem* m_videoSystem;

	/// Initializes the filesystem for save files/other data
	bool InitializeDataFilesystem();

	/// Adds log writers
	void AddLogWriters();

	/// Initializes the engine's systems in the right order
	bool InitializeSystems(const CVector<ISystem*>& systems);

	/// Pre frame stuff
	void PreFrame();

	/// Mid frame stuff
	void Update();

	/// Post frame stuff
	void PostFrame();

	/// Shuts down the engine's systems in the right order
	void ShutdownSystems();

	/// Check if the engine's state should change
	void CheckState();

	static constexpr LibDependency_t CLIENT_LIBS[] = {
		{"Rhi", true}
    };
	static constexpr SystemDependency CLIENT_SYSTEMS[] = {
		{ "VideoSystem",  IVideoSystem::VERSION, true, false},
        {"RenderSystem", IRenderSystem::VERSION, true, false}
    };
	static constexpr u32 CLIENT_SYSTEMS_OFFSET = 0;
};
