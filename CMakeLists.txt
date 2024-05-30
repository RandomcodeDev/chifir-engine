cmake_minimum_required(VERSION 3.25)

project(Красний VERSION 0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(USE_MIMALLOC "Use mimalloc instead of the system allocator" TRUE)
if (DEFINED NX OR DEFINED XENON)
	set(USE_MIMALLOC FALSE CACHE BOOL "mimalloc isn't supported on this platform" FORCE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(
	${CMAKE_SOURCE_DIR}
	$<$<BOOL:${USE_MIMALLOC}>:${CMAKE_SOURCE_DIR}/external/mimalloc/include>
	${CMAKE_SOURCE_DIR}/external/rtm/includes
	${CMAKE_SOURCE_DIR}/external/spdlog/include
	${CMAKE_SOURCE_DIR}/external/spdlog/include/spdlog
)

if ("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "AMD64|x86_64")
	set(PROCESSOR AMD64)
elseif ("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "86")
	set(PROCESSOR X86)
elseif ("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "aarch64|arm64")
	set(PROCESSOR ARM64)
endif()

add_compile_definitions(
	KR_PLATFORM_$<UPPER_CASE:${CMAKE_SYSTEM_NAME}>
	KR_PLATFORM="${CMAKE_SYSTEM_NAME}"
	KR_$<UPPER_CASE:$<CONFIG>>
	KR_CONFIGURATION="$<CONFIG>"
	KR_${PROCESSOR}
	KR_PROCESSOR="${PROCESSOR}"
	$<$<BOOL:${USE_MIMALLOC}>:KR_USE_MIMALLOC>
)

if (DEFINED XENON)
	add_compile_definitions(KR_PLATFORM_XBOX KR_PLATFORM_XBOX360)
endif()

if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES "Gaming")
	set(GDK TRUE)
	add_compile_definitions(KR_PLATFORM_GDK)
	if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES "Xbox")
		set(XBOX TRUE)
		add_compile_definitions(KR_PLATFORM_XBOX)
		if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES "Scarlett")
			add_compile_definitions(KR_PLATFORM_SCARLETT)
		else()
			add_compile_definitions(KR_PLATFORM_DURANGO)
		endif()
	endif()
endif()

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out/${CMAKE_SYSTEM_NAME}/${PROCESSOR}/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR} CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR} CACHE STRING "" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR} CACHE STRING "" FORCE)

add_subdirectory(external)
add_subdirectory(framework)
add_subdirectory(video)
add_subdirectory(render)
add_subdirectory(engine)
add_subdirectory(launcher)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Launcher)
