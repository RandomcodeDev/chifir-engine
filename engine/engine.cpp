#include "engine.h"
#include "base/filesystem.h"
#include "base/loader.h"
#include "base/log.h"
#include "rendersystem/irendersystem.h"
#include "rhi/rhi.h"
#include "videosystem/ivideosystem.h"

CEngine::CEngine() : m_state(EngineState_t::Uninitialized), m_headless(false), m_rhiBackend(DEFAULT_RHI_BACKEND), m_renderSystem(nullptr), m_videoSystem(nullptr)
{
}

CEngine::~CEngine()
{
}

void CEngine::Setup(const CVector<CString>& args)
{
	Log_Debug("Got %zd command line arguments", args.Size());

	for (ssize i = 0; i < args.Size(); i++)
	{
		Log_Debug("\t%s", args[i].Data());

		if (args[i] == "-headless")
		{
			m_headless = true;
		}

		if (args[i] == "-rhi_backend")
		{
			i++;
			Log_Debug("\t%s", args[i].Data());
			ssize endOffset = 0;
			m_rhiBackend = static_cast<RhiBackendType_t>(args[i].ParseInt(0, &endOffset));
			if (endOffset < 1) // wasn't valid
			{
				m_rhiBackend = Rhi_GetBackendTypeByName(args[i].Data());
			}
		}
	}
}

void CEngine::GetDependencies(CVector<SystemDependency_t>& systems, CVector<LibDependency_t>& libs)
{
	static const LibDependency_t CLIENT_LIBS[] = {
		{"Rhi", true}
    };
	static const SystemDependency_t CLIENT_SYSTEMS[] = {
		{ "VideoSystem",  IVideoSystem::VERSION, true, false},
        {"RenderSystem", IRenderSystem::VERSION, true, false}
    };

	if (!m_headless)
	{
		libs.Add(CLIENT_LIBS, ArraySize(CLIENT_LIBS));
		systems.Add(CLIENT_SYSTEMS, ArraySize(CLIENT_SYSTEMS));
	}
}

s32 CEngine::Run(const CVector<ISystem*>& systems)
{
	Log_Info("Engine compiled by " COMPILER " running on %s on %s", Plat_GetSystemDescription(), Plat_GetHardwareDescription());

	Log_Info("Initializing engine");
	m_state = EngineState_t::Startup;

	// TODO: make it so save filesystem is only open when necessary, it seems like console companies prefer that
	if (!InitializeSaveFilesystem())
	{
		Base_Quit("Failed to initialize filesystem!");
	}

	AddLogWriters();

	if (m_headless)
	{
		Log_Info("Running in headless mode");
	}

	if (!InitializeSystems(systems))
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
	// in general, it goes against
#if defined CH_DEBUG || !defined CH_CONSOLE
	Log_AddWriter(new CFileLogWriter(m_saveFilesystem, "chifir.log"));
#endif
}

bool CEngine::InitializeSystems(const CVector<ISystem*>& systems)
{
	Log_Debug("Initializing systems");

	// order has to match GetRequiredSystems

	if (!m_headless)
	{
		m_videoSystem = reinterpret_cast<IVideoSystem*>(systems[0]);
		if (!m_videoSystem->Initialize())
		{
			Log_FatalError("Video system initialization failed!");
			return false;
		}

		m_renderSystem = reinterpret_cast<IRenderSystem*>(systems[1]);
		if (!m_renderSystem->Initialize(m_videoSystem, m_rhiBackend))
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
