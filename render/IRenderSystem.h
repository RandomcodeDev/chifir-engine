/// @file IRenderSystem.h
/// @brief This file defines the render system interface
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"
#include "framework/ISystem.h"

class IRenderSystem : public ISystem
{
  public:
	virtual ~IRenderSystem() = default;

	/// @brief Initialize the render system
	virtual bool Initialize() = 0;

	/// @brief Wait for the GPU to finish things (must be called before Shutdown depending on the API)
	virtual void WaitForGpu() = 0;

	/// @brief Shut down the render system
	virtual void Shutdown() = 0;

	/// @brief Begin a frame
	virtual void BeginFrame() = 0;

	/// @brief End a frame
	virtual void EndFrame() = 0;

	/// @brief Clear colour
	virtual void ClearColour(rtm::vector4d colour = {0.0, 0.0, 0.0, 0.0}) = 0;

	/// @brief Get the name of the render system
	virtual const std::string& GetName() const = 0;

	/// @brief Get the name of the GPU
	virtual const std::string& GetGpuName() const = 0;

	static constexpr u32 VERSION = 1;
};
