/// \file Abstracts things for dxcommon.h
/// \copyright 2025 Randomcode Developers

#pragma once

#ifdef IN_RHI_DIRECTX12
#include <dxgi1_6.h>
#ifdef CH_DEBUG
#include <dxgidebug.h>
#endif
#include "d3d12.h"

class CDx12RhiDevice;
using CDxRhiDevice = CDx12RhiDevice;
class CDx12RhiRenderTarget;
using CDxRhiRenderTarget = CDx12RhiRenderTarget;
#elif defined IN_RHI_DIRECTX9
#include <d3d9.h>
#include <d3d9types.h>

class CDx9RhiDevice;
using CDxRhiDevice = CDx9RhiDevice;
class CDx9RhiRenderTarget;
using CDxRhiRenderTarget = CDx9RhiRenderTarget;
#elif defined IN_RHI_DIRECTX8
#include <d3d8.h>
#include <d3d8types.h>

class CDx8RhiDevice;
using CDxRhiDevice = CDx8RhiDevice;
class CDx8RhiRenderTarget;
using CDxRhiRenderTarget = CDx8RhiRenderTarget;
#else
#error "Not in a known legacy DirectX RHI backend"
#endif