/// \file Implementations for dxver.h
/// \copyright 2025 Randomcode Developers

#ifdef IN_RHI_DIRECTX9
#include "../dx9/device_dx9.h"
#include "../dx9/image_dx9.h"
#elif defined IN_RHI_DIRECTX8
#include "../dx8/device_dx8.h"
#include "../dx8/image_dx8.h"
#else
#error "Unknown DirectX backend"
#endif