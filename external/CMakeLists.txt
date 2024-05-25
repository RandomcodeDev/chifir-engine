cmake_minimum_required(VERSION 3.29)

if (${USE_MIMALLOC})
	add_subdirectory(mimalloc)
endif()

