cmake_minimum_required(VERSION 3.25)
set(PROJECT_VERSION "0.0.8")
if(DEFINED ENV{PROJECT_VERSION})
	set(PROJECT_VERSION "$ENV{PROJECT_VERSION}")
endif()
project(zBassMusic VERSION ${PROJECT_VERSION})

option(BUILD_VDF "Build .VDF file with plugin" ON)

set(CMAKE_CXX_STANDARD 23)
set(UNION_API_DIR "${CMAKE_SOURCE_DIR}/dependencies/union-api")
set(GOTHIC_API_DIR "${CMAKE_SOURCE_DIR}/gothic-api")
set(BASS_DIR "${CMAKE_SOURCE_DIR}/dependencies/bass")
set(VDF_DIR "${CMAKE_SOURCE_DIR}/vdf")

include(cmake/union-api.cmake)
include(cmake/gothic-api.cmake)
include(cmake/bass.cmake)

add_subdirectory(plugin)
