cmake_minimum_required(VERSION 3.25)

set(ENGINE_HEADERS
	CEngineApplication.h
	Engine.h
)
set(ENGINE_SOURCES
	CEngineApplication.cpp
	Interface.cpp
)

add_library(Engine SHARED)
target_sources(Engine PRIVATE ${ENGINE_HEADERS} ${ENGINE_SOURCES})

target_compile_definitions(Engine PRIVATE KR_ENGINE)

target_link_libraries(Engine PRIVATE Framework FrameworkMemoryOverride)

