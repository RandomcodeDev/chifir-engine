#include "engine.h"
#include "base/filesystem.h"
#include "base/log.h"
#include "videosystem/ivideosystem.h"

void CEngine::GetRequiredSystems(CVector<SystemDependency_t>& dependencies)
{
	static const SystemDependency_t deps[] = {
		{"VideoSystem", IVideoSystem::VERSION, false}
    };

	dependencies.Add(deps, ARRAY_SIZE(deps));
}

s32 CEngine::Run(const CVector<ISystem*>& systems)
{
	Log_Info("Initializing engine");
	m_state = EngineStateStartup;

	if (!InitializeSaveFilesystem())
	{
		Base_Quit("Failed to initialize filesystem!");
	}

	AddLogWriters();

	Log_Info("Engine running on %s on %s", Plat_GetSystemDescription(), Plat_GetHardwareDescription());

	m_videoSystem = reinterpret_cast<IVideoSystem*>(systems[0]);

	if (!InitializeSystems())
	{
		Base_Quit("Failed to initialize a system!");
	}

	Log_Info("Initialization done, entering main loop");
	m_state = EngineStateRunning;
	while (m_state >= EngineStateRunning)
	{
		CheckState();

		PreFrame();
		Update();
		PostFrame();
	}

	Log_Info("Shutting down");
	m_state = EngineStateShutdown;

	ShutdownSystems();

	Log_Info("Shut down finished, returning to launcher");

	return 0;
}

bool CEngine::InitializeSaveFilesystem()
{
	m_saveFilesystem = Base_CreateRawFilesystem(".");
	if (!m_saveFilesystem)
	{
		Log_Error("Failed to create raw filesystem with root \".\"");
		return false;
	}

	return true;
}

void CEngine::AddLogWriters()
{
	Log_AddWriter(new CFileLogWriter(m_saveFilesystem, "chifir.log"));
}

bool CEngine::InitializeSystems()
{
	if (!m_videoSystem->Initialize())
	{
		Log_FatalError("Video system initialization failed!");
		return false;
	}

	return true;
}

void CEngine::PreFrame()
{
	if (!m_videoSystem->Update())
	{
		m_state = EngineStateShutdown;
	}

	m_inFrame = true;
}

void CEngine::Update()
{
}

void CEngine::PostFrame()
{
	m_inFrame = false;
}

void CEngine::ShutdownSystems()
{
	m_videoSystem->Shutdown();
}

void CEngine::CheckState()
{
	// TODO: lock engine state here

	if (m_state == EngineStateRunning)
	{
		if (!m_videoSystem->Focused())
		{
			m_state = EngineStateInactive;
		}
	}

	if (m_state == EngineStateInactive)
	{
		if (m_videoSystem->Focused())
		{
			m_state = EngineStateRunning;
		}
	}

	// TODO: unlock engine state here
}
