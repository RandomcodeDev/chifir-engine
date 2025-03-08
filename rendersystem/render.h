#pragma once

#include "rendersystem/irendersystem.h"

#include "rhi/irhidevice.h"
#include "rhi/irhiimage.h"
#include "rhi/irhiinstance.h"
#include "rhi/irhiswapchain.h"

class IVideoSystem;

class CRenderSystem: public IRenderSystem
{
  public:
	CRenderSystem()
	{
	}
	virtual ~CRenderSystem() DEFAULT;

	virtual bool Initialize(IVideoSystem* videoSystem);
	virtual void Shutdown();

	virtual bool ChangeDevice(u32 index);

	virtual void BeginFrame();
	virtual void EndFrame();

  private:
	IRhiInstance* m_instance;
	IRhiDevice* m_device;
	IRhiSwapChain* m_swapChain;
	CVector<IRhiRenderTarget*> m_swapChainBuffers;
};
