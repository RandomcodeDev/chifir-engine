cmake_minimum_required(VERSION 3.25)

set(RENDER_HEADERS
	IRenderSystem.h
	impl/CNVRHIRenderSystem.h
	impl/INVRHIDeviceManager.h
)
set(RENDER_DX12_HEADERS
	impl/CDX12NVRHIDeviceManager.h
)
set(RENDER_SOURCES
	impl/Interface.cpp
	impl/CNVRHIRenderSystem.cpp
)
set(RENDER_DX12_SOURCES
	impl/CDX12NVRHIDeviceManager.cpp
)

add_library(RenderSystem SHARED)
target_sources(RenderSystem PRIVATE ${RENDER_HEADERS} ${RENDER_SOURCES})
if (DEFINED WIN32 AND ${CMAKE_SYSTEM_VERSION} VERSION_GREATER_EQUAL 7.0)
	set(DX12 TRUE)
	target_sources(RenderSystem PRIVATE ${RENDER_DX12_HEADERS} ${RENDER_DX12_SOURCES})
	target_compile_definitions(RenderSystem PRIVATE KR_DX12=1)
	target_link_libraries(RenderSystem PRIVATE d3d12.lib dxgi.lib dxguid.lib)
endif()

target_link_libraries(RenderSystem PRIVATE Framework FrameworkMemoryOverride)

if (DEFINED GDK)
	set_gdk_properties(RenderSystem)
endif()

