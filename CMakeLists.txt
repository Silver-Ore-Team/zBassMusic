cmake_minimum_required(VERSION 3.25)
project(zBassMusic)

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