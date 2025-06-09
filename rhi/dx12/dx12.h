/// \file DirectX 12 definitions
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/meta.h"
#include "base/platform.h"

#include "rhi/rhi.h"

#ifdef CH_SCARLETT
#define DX12_TARGET_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_2
#else
#define DX12_TARGET_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_1
#define DX12_BACKUP_FEATURE_LEVEL D3D_FEATURE_LEVEL_11_0
#endif

#include "dxcommon.h"
