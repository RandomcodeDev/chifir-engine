#include "framework/Log.h"

#include "CDirectX12RenderSystem.h"

bool CDirectX12RenderSystem::Initialize()
{
	KR_LOG_INFO("Initializing DirectX 12 render system");

	if (!CreateFactory() || !FindAdapter() || !CreateDevice())
	{
		return false;
	}

	KR_LOG_INFO("DirectX 12 render system initialized");
	return true;
}

void CDirectX12RenderSystem::WaitForGpu()
{
}

void CDirectX12RenderSystem::Shutdown()
{
	KR_LOG_INFO("Shutting down DirectX 12 render system");



	KR_LOG_INFO("DirectX 12 render system shut down");
}

void CDirectX12RenderSystem::BeginFrame()
{
}

void CDirectX12RenderSystem::EndFrame()
{
}

void CDirectX12RenderSystem::ClearColour(rtm::vector4d colour)
{
}

void CDirectX12RenderSystem::EnableDebug()
{
#ifdef KR_DEBUG
#endif
}

bool CDirectX12RenderSystem::CreateFactory()
{
	return true;
}

bool CDirectX12RenderSystem::FindAdapter()
{
	return true;
}

bool CDirectX12RenderSystem::CreateDevice()
{
	return true;
}
