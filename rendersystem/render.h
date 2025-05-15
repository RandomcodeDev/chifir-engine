/// \file Render system implementation
/// \copyright 2025 Randomcode Developers

#pragma once

#include "rendersystem/irendersystem.h"

#include "rhi/irhicommandlist.h"
#include "rhi/irhidevice.h"
#include "rhi/irhiimage.h"
#include "rhi/irhiinstance.h"
#include "rhi/irhiswapchain.h"
#include "rhi/rhi.h"

class IVideoSystem;

class CRenderSystem: public IRenderSystem
{
  public:
	CRenderSystem()
	{
	}
	virtual ~CRenderSystem() = default;

	virtual bool Initialize(IVideoSystem* videoSystem, cstr backendName);
	virtual void Shutdown();

	virtual bool ChangeDevice(u32 index);

	virtual void BeginFrame();
	virtual void EndFrame();

  private:
	IVideoSystem* m_videoSystem;
	IRhiInstance* m_instance;
	IRhiDevice* m_device;
	IRhiSwapChain* m_swapChain;
	CVector<IRhiRenderTarget*> m_swapChainBuffers;
	IRhiCommandList* m_commandList;
};
