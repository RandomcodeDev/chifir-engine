#include "framework/ISystem.h"

#include "CEngineApplication.h"

std::vector<SystemDependency> CEngineApplication::GetSystems() const
{
	return std::vector<SystemDependency>();
}

void CEngineApplication::Run(CCommandLine* cmdLine)
{
}
