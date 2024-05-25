cmake_minimum_required(VERSION 3.29)

set(LAUNCHER_SOURCES
	entry.cpp
)

add_executable(Launcher)
target_sources(Launcher PRIVATE ${LAUNCHER_SOURCES})

target_link_libraries(Launcher PRIVATE Framework FrameworkMemoryOverride)

