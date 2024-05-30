/// @file Engine.h
/// @brief This file has definitions used in the engine.
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "framework/Framework.h"

class IRenderSystem;
class IVideoSystem;

extern IRenderSystem* g_renderSystem;
extern IVideoSystem* g_videoSystem;
