cmake_minimum_required(VERSION 3.25)
project(zBassMusic VERSION "0.1.2")

option(BUILD_VDF "Build .VDF file with plugin" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

set(UNION_API_DIR "${CMAKE_SOURCE_DIR}/dependencies/union-api")
set(GOTHIC_API_DIR "${CMAKE_SOURCE_DIR}/dependencies/gothic-api")
set(BASS_DIR "${CMAKE_SOURCE_DIR}/dependencies/bass")
set(VDF_DIR "${CMAKE_SOURCE_DIR}/vdf")
set(GOTHIC_USERAPI_DIR "${CMAKE_SOURCE_DIR}/gothic-userapi")

include(cmake/union-api.cmake)
include(cmake/gothic-api.cmake)
include(cmake/bass.cmake)

add_subdirectory(plugin)
