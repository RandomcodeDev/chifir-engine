cmake_minimum_required(VERSION 3.25)

project(Красний VERSION 0.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(USE_MIMALLOC "Use mimalloc instead of the system allocator" TRUE)
if (DEFINED NX OR DEFINED XENON)
	set(USE_MIMALLOC FALSE CACHE BOOL "mimalloc isn't supported on this platform" FORCE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)

include_directories(
	${CMAKE_SOURCE_DIR}
	$<$<BOOL:${USE_MIMALLOC}>:${CMAKE_SOURCE_DIR}/external/mimalloc/include>
	${CMAKE_SOURCE_DIR}/external/nvrhi/include
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

if (DEFINED WIN32 AND ${CMAKE_SYSTEM_VERSION} VERSION_GREATER_EQUAL 7.0)
	set(DX12 TRUE)
endif()

if (NOT DEFINED APPLE)
	set(VULKAN TRUE)
endif()

if (DEFINED CMAKE_VS_PLATFORM_NAME)
	add_compile_definitions(KR_VS_PLATFORM="${CMAKE_VS_PLATFORM_NAME}")
endif()

add_compile_definitions(
	KR_PLATFORM_$<UPPER_CASE:${CMAKE_SYSTEM_NAME}>
	KR_PLATFORM="${CMAKE_SYSTEM_NAME}"
	KR_$<UPPER_CASE:$<CONFIG>>
	KR_CONFIGURATION="$<CONFIG>"
	KR_${PROCESSOR}
	KR_PROCESSOR="${PROCESSOR}"
	$<$<BOOL:${USE_MIMALLOC}>:KR_USE_MIMALLOC>
	$<$<BOOL:${DX12}>:KR_DX12>
	$<$<BOOL:${VULKAN}>:KR_VULKAN>
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

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	add_compile_options(-Wno-reserved-identifier -Wno-unsafe-buffer-usage)
endif()

add_subdirectory(external)
add_subdirectory(framework)
add_subdirectory(video)
add_subdirectory(render)
add_subdirectory(engine)
add_subdirectory(launcher)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Launcher)
