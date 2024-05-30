cmake_minimum_required(VERSION 3.25)

set(RENDER_HEADERS
	IRenderSystem.h
)
set(RENDER_DX12_HEADERS
		impl/dx12/CDirectX12RenderSystem.h
)
set(RENDER_SOURCES
	impl/Interface.cpp
)
set(RENDER_DX12_SOURCES
	impl/dx12/CDirectX12RenderSystem.cpp
)

add_library(RenderSystem SHARED)
target_sources(RenderSystem PRIVATE ${RENDER_HEADERS} ${RENDER_SOURCES})
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND ${CMAKE_SYSTEM_VERSION} VERSION_GREATER_EQUAL 7.0)
	target_compile_definitions(RenderSystem PRIVATE KR_RENDER_DX12)
	target_sources(RenderSystem PRIVATE ${RENDER_DX12_HEADERS} ${RENDER_DX12_SOURCES})
	target_link_libraries(RenderSystem PRIVATE DirectX-Headers DirectX-Guids)
endif()

target_link_libraries(RenderSystem PRIVATE Framework FrameworkMemoryOverride)

if (DEFINED GDK)
	set_gdk_properties(RenderSystem)
endif()

