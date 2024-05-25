cmake_minimum_required(VERSION 3.29)

project(Krasnyy VERSION 0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(USE_MIMALLOC "Use mimalloc instead of the system allocator" ON)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "NX")
	set(USE_MIMALLOC FALSE CACHE BOOL "mimalloc isn't supported on this platform" FORCE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(${CMAKE_SOURCE_DIR})

add_compile_definitions(KR_PLATFORM_$<UPPER_CASE:${CMAKE_SYSTEM_NAME}> KR_PLATFORM="${CMAKE_SYSTEM_NAME}" KR_$<UPPER_CASE:$<CONFIG>> KR_CONFIGURATION="$<CONFIG>")

add_subdirectory(external)
add_subdirectory(framework)
add_subdirectory(launcher)

