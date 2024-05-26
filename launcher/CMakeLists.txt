cmake_minimum_required(VERSION 3.29)

set(LAUNCHER_SOURCES
	entry.cpp
)

add_executable(Launcher)
target_sources(Launcher PRIVATE ${LAUNCHER_SOURCES})

target_link_libraries(Launcher PRIVATE Framework FrameworkMemoryOverride)

if (DEFINED GDK)
	add_custom_command(TARGET Launcher PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/gdk/MicrosoftGameConfig.mgc ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MicrosoftGame.Config)
endif()

