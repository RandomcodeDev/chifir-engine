cmake_minimum_required(VERSION 3.25)

set(FRAMEWORK_HEADERS
	CCommandLine.h
	Framework.h
	IApplication.h
	IPlatform.h
	ISharedLibrary.h
	ISystem.h
	Log.h
	Memory.h
	Util.h
)
set(FRAMEWORK_Windows_HEADERS
	impl/windows/CWindowsPlatform.h
	impl/windows/CWindowsSharedLibrary.h
)
set(FRAMEWORK_SOURCES
	impl/CCommandLine.cpp
	impl/Log.cpp
	impl/Memory.cpp
	MemoryOverride.cpp
)
set(FRAMEWORK_Windows_SOURCES
	impl/windows/CWindowsPlatform.cpp
	impl/windows/CWindowsSharedLibrary.cpp
)

add_library(Framework SHARED)
target_sources(Framework PRIVATE ${FRAMEWORK_HEADERS} ${FRAMEWORK_SOURCES} ${FRAMEWORK_${CMAKE_SYSTEM_NAME}_HEADERS} ${FRAMEWORK_${CMAKE_SYSTEM_NAME}_SOURCES})

target_compile_definitions(Framework PRIVATE KR_FRAMEWORK)

add_library(FrameworkMemoryOverride OBJECT)
target_sources(FrameworkMemoryOverride PRIVATE MemoryOverride.cpp)

if (${USE_MIMALLOC})
	target_link_libraries(Framework PRIVATE mimalloc-static)
endif()

if (WIN32 AND NOT DEFINED XBOX)
	target_link_libraries(Framework PRIVATE dbghelp.lib)
endif()

if (DEFINED GDK)
	target_link_libraries(Framework PRIVATE XGameRuntime.lib)
endif()

