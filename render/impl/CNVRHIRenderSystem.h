/// @file CNVRHIRenderSystem.h
/// @brief This file declares the NVRHI-based implementation of IRenderSystem
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "INVRHIDeviceManager.h"
#include "framework/Framework.h"
#include "framework/Log.h"
#include "framework/Util.h"
#include "render/IRenderSystem.h"
#include "video/IVideoSystem.h"

class CNVRHIRenderSystem : public IRenderSystem
{
  public:
	bool Initialize(IVideoSystem* videoSystem);
	void WaitForGpu();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	void ClearColour(rtm::vector4d colour);

	const std::string& GetName() const
	{
		return m_apiName;
	}

	const std::string& GetGpuName() const
	{
		return m_deviceMgr->GetGpuName();
	}

  private:
	IVideoSystem* m_videoSystem;
	std::unique_ptr<INVRHIDeviceManager> m_deviceMgr;
	nvrhi::DeviceHandle m_device;
	std::string m_apiName;

	bool CreateCommandLists();
};
