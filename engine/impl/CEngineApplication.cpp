#include "framework/IPlatform.h"
#include "framework/ISystem.h"
#include "framework/Util.h"

#include "video/IVideoSystem.h"

#include "CEngineApplication.h"

std::vector<SystemDependency> CEngineApplication::GetSystems() const
{
	std::vector<SystemDependency> systems;

	systems.push_back(SystemDependency{"Video", 1, false, false});
	//systems.push_back(SystemDependency{"Render", 1, false, false});

	return systems;
}

void CEngineApplication::Run(const CCommandLine& cmdLine, const std::unordered_map<std::string, ISystem*>& systems)
{
	g_videoSystem = reinterpret_cast<IVideoSystem*>(systems.at("Video"));

	if (!g_videoSystem->Initialize())
	{
		KR_QUIT("Failed to initialize video system!");
	}

	g_videoSystem->SetTitle(fmt::format("{} | " KR_PROCESSOR " " KR_CONFIGURATION " | {}", GetName(), GetPlatform()->DescribeOs()));

	m_running = true;
	Loop();

	g_videoSystem->Shutdown();
}

void CEngineApplication::Loop()
{
	while (m_running)
	{
		m_running = g_videoSystem->Update();
	}
}
