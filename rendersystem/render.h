#pragma once

#include "rendersystem/irendersystem.h"

class IRhiInstance;
class IRhiDevice;
class IVideoSystem;

class CRenderSystem: public IRenderSystem
{
  public:
	CRenderSystem()
	{
	}
	virtual ~CRenderSystem() DEFAULT;

	bool Initialize(IVideoSystem* videoSystem);
	void Shutdown();

	void BeginFrame();
	void EndFrame();

  private:
	IRhiInstance* m_instance;
	IRhiDevice* m_device;
};
