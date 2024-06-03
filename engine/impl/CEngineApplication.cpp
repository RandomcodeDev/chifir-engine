#include "framework/IPlatform.h"
#include "framework/ISystem.h"
#include "framework/Util.h"

#include "render/IRenderSystem.h"
#include "video/IVideoSystem.h"

#include "CEngineApplication.h"

std::vector<SystemDependency> CEngineApplication::GetSystems() const
{
	std::vector<SystemDependency> systems;

	systems.push_back(SystemDependency{"Video", 1, false, false});
	systems.push_back(SystemDependency{"Render", 1, false, false});

	return systems;
}

void CEngineApplication::Run(const CCommandLine& cmdLine, const std::unordered_map<std::string, ISystem*>& systems)
{
	g_videoSystem = reinterpret_cast<IVideoSystem*>(systems.at("Video"));
	g_renderSystem = reinterpret_cast<IRenderSystem*>(systems.at("Render"));

	if (!g_videoSystem->Initialize())
	{
		KR_QUIT("Failed to initialize video system!");
	}

	if (!g_renderSystem->Initialize())
	{
		KR_QUIT("Failed to initialize render system!");
	}

	g_videoSystem->SetTitle(fmt::format(
		"{} | " KR_PROCESSOR " " KR_CONFIGURATION " | {} | {} renderer using {}", GetName(), GetPlatform()->DescribeOs(), g_renderSystem->GetName(),
		g_renderSystem->GetGpuName()));

	m_running = true;
	Loop();

	g_renderSystem->WaitForGpu();
	g_renderSystem->Shutdown();

	g_videoSystem->Shutdown();
}

void CEngineApplication::Loop()
{
	while (m_running)
	{
		m_running = g_videoSystem->Update();

		g_renderSystem->BeginFrame();
		g_renderSystem->ClearColour();
		g_renderSystem->EndFrame();
	}
}
