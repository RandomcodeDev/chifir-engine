/// \file Engine core implementation
/// \copyright 2025 Randomcode Developers

#include "base/filesystem.h"
#include "base/loader.h"
#include "base/log.h"
#include "base/platform.h"

#include "math/math.h"
#include "math/vec3.h"

#include "rendersystem/irendersystem.h"

#include "rhi/rhi.h"

#include "videosystem/ivideosystem.h"

#include "engine.h"

CEngine::CEngine() : m_state(EngineState_t::Uninitialized), m_headless(false), m_renderSystem(nullptr), m_videoSystem(nullptr)
{
#ifdef CH_WIN32
#if defined CH_IA32 && defined CH_UWP
	if (Plat_IsUwpApp())
#endif
	{
		m_rhiBackendName = "DirectX12";
	}
#ifdef CH_IA32
#ifdef CH_UWP
	else
#endif
	{
		// for legacy support
		m_rhiBackendName = "DirectX9";
	}
#endif
#elif defined CH_XENON
	m_rhiBackendName = "DirectX9";
#elif defined CH_XBOX
	m_rhiBackendName = "DirectX8";
#else
	m_rhiBackendName = "Vulkan";
#endif
}

CEngine::~CEngine()
{
}

void CEngine::Setup(const CVector<CString>& args)
{
	// TODO: implement and use CCommandLine
	Log_Debug("Got %zd command line arguments", args.Size());

	for (ssize i = 0; i < args.Size(); i++)
	{
		Log_Debug("\t%s", args[i].Data());

		if (args[i] == "-headless")
		{
			m_headless = true;
		}

		if (args[i] == "-rhi_backend" && !m_headless)
		{
			i++;
			Log_Debug("\t%s", args[i].Data());
			m_rhiBackendName = args[i].Data();
		}
	}
}

void CEngine::GetDependencies(CVector<SystemDependency>& systems, CVector<LibDependency_t>& libs)
{
	if (!m_headless)
	{
		libs.Add(CLIENT_LIBS, ArraySize(CLIENT_LIBS));
		systems.Add(CLIENT_SYSTEMS, ArraySize(CLIENT_SYSTEMS));
	}
}

s32 CEngine::Run(const CVector<ISystem*>& systems)
{
	Log_Info("Initializing engine");
	m_state = EngineState_t::Startup;

	Plat_GetDateTime(m_startTime);

	// TODO: make it so save filesystem is only open when necessary, it seems like console companies prefer that
	// maybe on platforms that are picky about that, since writes are uncommon, mount and unmount at every write
	if (!InitializeDataFilesystem())
	{
		Base_Quit("Failed to initialize filesystem!");
	}

	AddLogWriters();

	Log_Info(
		PLAT_NAME " " ARCH_NAME " " CONFIG_NAME " engine, branch " GIT_BRANCH " commit " GIT_COMMIT
				  ", compiled by %s, running on %s on %s",
		COMPILER_VERSION(), Plat_GetSystemDescription(), Plat_GetHardwareDescription());

	Vec3f a(1, 2, 3);
	Vec3f b(2, 4, 6);

	Log_Info("a = [%f %f %f], |a| = %f", a[0], a[1], a[2], a.Length());
	Log_Info("b = [%f %f %f], |b| = %f", b[0], b[1], b[2], b.Length());
	Vec3f a2 = a * 2;
	Log_Info("a * 2 = [%f %f %f]", a2[0], a2[1], a2[2]);
	Vec3f b2 = b / 2;
	Log_Info("b / 2 = [%f %f %f]", b2[0], b2[1], b2[2]);
	Log_Info("a * b = %f", a.Dot(b));
	Vec3f c = a.Cross(b);
	Log_Info("a x b = [%f %f %f]", c[0], c[1], c[2]);

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

bool CEngine::InitializeDataFilesystem()
{
	Log_Debug("Opening engine data filesystem %s", Plat_GetDataLocation());
	m_dataFilesystem = Base_CreateRawFilesystem(Plat_GetDataLocation());
	if (!m_dataFilesystem)
	{
		Log_Error("Failed to create raw filesystem with root \"%s\"", Plat_GetDataLocation());
		return false;
	}

	return true;
}

void CEngine::AddLogWriters()
{
	// in general, it goes against
#if defined CH_DEBUG || !defined CH_CONSOLE
	Log_AddWriter(new CFileLogWriter(m_dataFilesystem, GAME_NAME, true));
#endif
}

bool CEngine::InitializeSystems(const CVector<ISystem*>& systems)
{
	Log_Debug("Initializing systems");

	// order has to match GetRequiredSystems

	if (!m_headless)
	{
		m_videoSystem = reinterpret_cast<IVideoSystem*>(systems[CLIENT_SYSTEMS_OFFSET + 0]);
		if (!m_videoSystem->Initialize())
		{
			Log_FatalError("Video system initialization failed!");
			return false;
		}

		m_renderSystem = reinterpret_cast<IRenderSystem*>(systems[CLIENT_SYSTEMS_OFFSET + 1]);
		if (!m_renderSystem->Initialize(m_videoSystem, m_rhiBackendName))
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
