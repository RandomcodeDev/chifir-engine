cmake_minimum_required(VERSION 3.29)

project(Krasnyy VERSION 0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(USE_MIMALLOC "Use mimalloc instead of the system allocator" TRUE)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "NX")
	set(USE_MIMALLOC FALSE CACHE BOOL "mimalloc isn't supported on this platform" FORCE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(
	${CMAKE_SOURCE_DIR}
	$<$<BOOL:${USE_MIMALLOC}>:${CMAKE_SOURCE_DIR}/external/mimalloc/include>
)

add_compile_definitions(
	KR_PLATFORM_$<UPPER_CASE:${CMAKE_SYSTEM_NAME}>
	KR_PLATFORM="${CMAKE_SYSTEM_NAME}"
	KR_$<UPPER_CASE:$<CONFIG>>
	KR_CONFIGURATION="$<CONFIG>"
	$<$<BOOL:${USE_MIMALLOC}>:KR_USE_MIMALLOC>
)

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

add_subdirectory(external)
add_subdirectory(framework)
add_subdirectory(launcher)

set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT Launcher)

