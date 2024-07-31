#include "engine.h"
#include "utility/log.h"
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

	m_videoSystem = reinterpret_cast<IVideoSystem*>(systems[0]);

	if (!InitializeSystems())
	{
		Util_Fatal("Failed to initialize a system!");
	}

	Log_Info("Initialization done, entering main loop");
	m_state = EngineStateRunning;
	while (m_state >= EngineStateRunning)
	{
		if (!m_videoSystem->Update())
		{
			m_state = EngineStateShutdown;
		}
	}

	Log_Info("Shutting down");
	m_state = EngineStateShutdown;

	ShutdownSystems();

	return 0;
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

void CEngine::ShutdownSystems()
{
	m_videoSystem->Shutdown();
}
