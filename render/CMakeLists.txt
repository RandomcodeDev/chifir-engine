cmake_minimum_required(VERSION 3.25)

set(RENDER_HEADERS
	IRenderSystem.h
	impl/CNVRHIRenderSystem.h
	impl/INVRHIDeviceManager.h
)
set(RENDER_SOURCES
	impl/Interface.cpp
	impl/CNVRHIRenderSystem.cpp
)

add_library(RenderSystem SHARED)
target_sources(RenderSystem PRIVATE ${RENDER_HEADERS} ${RENDER_SOURCES})

target_link_libraries(RenderSystem PRIVATE Framework FrameworkMemoryOverride)

if (DEFINED GDK)
	set_gdk_properties(RenderSystem)
endif()

