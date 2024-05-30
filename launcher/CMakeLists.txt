cmake_minimum_required(VERSION 3.25)

set(LAUNCHER_SOURCES
	Launcher.cpp
)
set(LAUNCHER_Windows_SOURCES
	Launcher.exe.manifest
)

add_executable(Launcher)
target_sources(Launcher PRIVATE ${LAUNCHER_SOURCES})

target_link_libraries(Launcher PRIVATE Framework FrameworkMemoryOverride)

# So when I press F5 I don't have to care whether I've built them
add_dependencies(Launcher Engine RenderSystem VideoSystem)

if (DEFINED GDK)
	set_gdk_properties(Launcher)
	add_custom_command(TARGET Launcher PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/gdk/MicrosoftGameConfig.mgc ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MicrosoftGame.Config)
endif()
