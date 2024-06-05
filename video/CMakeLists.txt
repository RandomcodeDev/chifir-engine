cmake_minimum_required(VERSION 3.25)

set(VIDEO_HEADERS
	IVideoSystem.h
)
set(VIDEO_Windows_HEADERS
	impl/windows/CWindowsVideoSystem.h
)
set(VIDEO_SOURCES
	impl/Interface.cpp
)
set(VIDEO_Windows_SOURCES
	impl/windows/CWindowsVideoSystem.cpp
)

add_library(VideoSystem SHARED)
target_sources(VideoSystem PRIVATE ${VIDEO_HEADERS} ${VIDEO_${CMAKE_SYSTEM_NAME}_HEADERS} ${VIDEO_SOURCES} ${VIDEO_${CMAKE_SYSTEM_NAME}_SOURCES})

target_link_libraries(VideoSystem PRIVATE Framework FrameworkMemoryOverride)

set_target_properties(VideoSystem PROPERTIES FOLDER Systems)

if (DEFINED GDK)
	set_gdk_properties(VideoSystem)
endif()

