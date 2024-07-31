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

	Log_Info("Initializing systems");

	m_videoSystem = reinterpret_cast<IVideoSystem*>(systems[0]);
	if (!m_videoSystem->Initialize())
	{
		Util_Fatal("Failed to initialize video system!");
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

	m_videoSystem->Shutdown();

	return 0;
}
