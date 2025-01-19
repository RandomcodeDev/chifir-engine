#include "engine.h"
#include "base/filesystem.h"
#include "base/log.h"
#include "rendersystem/irendersystem.h"
#include "videosystem/ivideosystem.h"

CEngine::CEngine() : m_state(EngineState_t::Uninitialized), m_headless(false), m_renderSystem(nullptr), m_videoSystem(nullptr)
{
}

CEngine::~CEngine()
{
}

void CEngine::Setup(const CVector<CString>& args)
{
	for (ssize i = 0; i < args.Size(); i++)
	{
		if (args[i] == "-headless")
		{
			m_headless = true;
		}
	}
}

void CEngine::GetRequiredSystems(CVector<SystemDependency_t>& dependencies)
{
	static const SystemDependency_t CLIENT_DEPS[] = {
		{ "VideoSystem",  IVideoSystem::VERSION, true, false},
        {"RenderSystem", IRenderSystem::VERSION, true, false}
    };

	if (!m_headless)
	{
		dependencies.Add(CLIENT_DEPS, ARRAY_SIZE(CLIENT_DEPS));
	}
}

s32 CEngine::Run(const CVector<ISystem*>& systems)
{
	Log_Info("Initializing engine");
	m_state = EngineState_t::Startup;

	if (!InitializeSaveFilesystem())
	{
		Base_Quit("Failed to initialize filesystem!");
	}

	AddLogWriters();

	Log_Info("Engine compiled by " COMPILER " running on %s on %s", Plat_GetSystemDescription(), Plat_GetHardwareDescription());
	if (m_headless)
	{
		Log_Info("Running in headless mode");
	}

	// same order as GetRequiredSystems
	if (!m_headless)
	{
		m_videoSystem = static_cast<IVideoSystem*>(systems[0]);
		m_renderSystem = static_cast<IRenderSystem*>(systems[1]);
	}

	if (!InitializeSystems())
	{
		Base_Quit("Failed to initialize a system!");
	}

	Log_Info("Initialization done, entering main loop");
	m_state = EngineState_t::Running;
	while (m_state >= EngineState_t::Running)
	{
		CheckState();

		PreFrame();
		Update();
		PostFrame();
	}

	Log_Info("Shutting down");
	m_state = EngineState_t::Shutdown;

	ShutdownSystems();

	Log_Info("Shut down finished, returning to launcher");

	return 0;
}

bool CEngine::InitializeSaveFilesystem()
{
	Log_Debug("Opening save filesystem %s", Plat_GetSaveLocation());
	m_saveFilesystem = Base_CreateRawFilesystem(Plat_GetSaveLocation());
	if (!m_saveFilesystem)
	{
		Log_Error("Failed to create raw filesystem with root \"%s\"", Plat_GetSaveLocation());
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
	Log_Debug("Initializing systems");

	if (!m_headless)
	{
		if (!m_videoSystem->Initialize())
		{
			Log_FatalError("Video system initialization failed!");
			return false;
		}

		if (!m_renderSystem->Initialize(m_videoSystem))
		{
			Log_FatalError("Render system initialization failed!");
			return false;
		}
	}

	return true;
}

void CEngine::PreFrame()
{
	if (!m_headless)
	{
		if (!m_videoSystem->Update())
		{
			m_state = EngineState_t::Shutdown;
		}

		m_renderSystem->BeginFrame();
	}

	m_inFrame = true;
}

void CEngine::Update()
{
}

void CEngine::PostFrame()
{
	if (!m_headless)
	{
		m_renderSystem->EndFrame();
	}

	m_inFrame = false;
}

void CEngine::ShutdownSystems()
{
	Log_Debug("Shutting down systems");
	if (!m_headless)
	{
		m_renderSystem->Shutdown();
		m_videoSystem->Shutdown();
	}
}

void CEngine::CheckState()
{
	// TODO: lock engine state here

	if (!m_headless)
	{
		if (m_state == EngineState_t::Running)
		{
			if (!m_videoSystem->Focused())
			{
				m_state = EngineState_t::Inactive;
			}
		}

		if (m_state == EngineState_t::Inactive)
		{
			if (m_videoSystem->Focused())
			{
				m_state = EngineState_t::Running;
			}
		}
	}

	// TODO: unlock engine state here
}
