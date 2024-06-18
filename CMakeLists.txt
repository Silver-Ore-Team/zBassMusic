cmake_minimum_required(VERSION 3.25..3.29.5)
set(PROJECT_VERSION "0.1.3")
set(PROJECT_VERSION_CMAKE "0.3.2")
set(VCPKG_TARGET_ARCHITECTURE "x86")
set(VCPKG_PLATFORM_TOOLSET "v143")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(zBassMusic VERSION "${PROJECT_VERSION_CMAKE}" LANGUAGES CXX)

option(BUILD_VDF "Build .VDF file with plugin" ON)
option(BUILD_TESTS "Build tests" ON)

set(BASS_DIR "${CMAKE_SOURCE_DIR}/dependencies/bass")
set(VDF_DIR "${CMAKE_SOURCE_DIR}/vdf")
set(GOTHIC_USERAPI_DIR "${CMAKE_SOURCE_DIR}/gothic-userapi")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAE_CXX_FLAGS_DEBUG} /Zc:__cplusplus")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAE_CXX_FLAGS_RELEASE} /Zc:__cplusplus")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAE_CXX_FLAGS_RELWITHDEBINFO} /Zc:__cplusplus")

include(FetchContent)
set(BUILD_UNION_API_STATIC OFF CACHE INTERNAL "Disable static build of UnionAPI")
FetchContent_Declare(
        UnionAPI
        URL https://github.com/piotrmacha/union-api.cmake/releases/latest/download/UnionAPI-v143-windows-2022.zip)
FetchContent_MakeAvailable(UnionAPI)
FetchContent_GetProperties(UnionAPI SOURCE_DIR UnionAPI_SOURCE_DIR)
set(CMAKE_FIND_PACKAGE_REDIRECTS_DIR ${UnionAPI_SOURCE_DIR})
set(UnionAPI_DIR ${UnionAPI_SOURCE_DIR}/lib/cmake/UnionAPI)
find_package(UnionAPI CONFIG REQUIRED)

find_package(spdlog CONFIG REQUIRED)
include(cmake/bass.cmake)

add_library(zbassmusic SHARED)
set_target_properties(zbassmusic PROPERTIES
        OUTPUT_NAME "zBassMusic"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}")

string(TIMESTAMP BUILD_TIME "%Y-%m-%dT%H:%M:%S")
execute_process(COMMAND git symbolic-ref --short HEAD OUTPUT_VARIABLE BUILD_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git log --format=%H -n 1 OUTPUT_VARIABLE BUILD_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/version.rc" @ONLY)
configure_file("${CMAKE_SOURCE_DIR}/vdf/script.vdfs.in" "${CMAKE_BINARY_DIR}/script.vdfs")
configure_file("${CMAKE_SOURCE_DIR}/src/BuildInfo.h.in" "${CMAKE_BINARY_DIR}/src/BuildInfo.h" @ONLY)

add_library(zbassmusic_gothicapi STATIC)
target_compile_options(zbassmusic_gothicapi PRIVATE /W1 /wd4530 /wd4005)
target_compile_definitions(zbassmusic_gothicapi PRIVATE __G1 __G1A __G2 __G2A)
target_link_libraries(zbassmusic_gothicapi PRIVATE UnionAPI::GothicAPI)

file(GLOB_RECURSE ZBASSMUSIC_SOURCES "src/**.cpp")
target_sources(zbassmusic PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/version.rc" ${ZBASSMUSIC_SOURCES})
target_compile_options(zbassmusic PRIVATE /W4 /WX /wd4530 /wd4005)
target_compile_definitions(zbassmusic PRIVATE __G1 __G1A __G2 __G2A)
target_include_directories(zbassmusic PRIVATE "src/" "${CMAKE_BINARY_DIR}/src/" "gothic-userapi"
        "${CMAKE_BINARY_DIR}/_deps/unionapi-src/include"
        "${CMAKE_BINARY_DIR}/_deps/unionapi-src/include/ZenGin/Gothic_UserAPI")
target_link_libraries(zbassmusic PRIVATE UnionAPI::UnionAPI zbassmusic_gothicapi bass_all)

if (${BUILD_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif ()

install(FILES $<TARGET_RUNTIME_DLLS:zbassmusic> DESTINATION ${CMAKE_BINARY_DIR})
install(FILES $<TARGET_RUNTIME_DLLS:zbassmusic> TYPE BIN)
install(TARGETS zbassmusic
        EXPORT zBassMusicTargets
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION bin
        RUNTIME DESTINATION bin)
install(EXPORT zBassMusicTargets
        FILE zBassMusicTargets.cmake
        DESTINATION lib/cmake/Plugin)

if (${BUILD_VDF})
    install(FILES "${CMAKE_BINARY_DIR}/script.vdfs" DESTINATION "${CMAKE_INSTALL_PREFIX}")
    install(SCRIPT "${VDF_DIR}/vdf.cmake")
    install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/vdf/GothicVDFS.exe /B \"${CMAKE_INSTALL_PREFIX}/script.vdfs\"
            WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}\")")
endif ()

if (${BUILD_VDF})
    string(REPLACE "build" "install" INSTALL_DIR "${CMAKE_BINARY_DIR}")
    add_custom_target(zbassmusic_vdf ALL
            COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${INSTALL_DIR}
            DEPENDS zbassmusic
            COMMENT "Run install to build VDF")

    if (DEFINED ENV{COPY_VDF_TARGET})
        add_custom_target(copy_vdf ALL
                COMMAND ${CMAKE_COMMAND} -E copy "${INSTALL_DIR}/zBassMusic.vdf" "$ENV{COPY_VDF_TARGET}"
                DEPENDS zbassmusic_vdf
                COMMENT "Copy zbassmusic to target directory: $ENV{COPY_VDF_TARGET}")
    endif ()
endif ()

if (DEFINED ENV{COPY_DLL_TARGET})
    add_custom_target(copy_dll ALL
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/zBassMusic.dll" "$ENV{COPY_DLL_TARGET}/Autorun/zBassMusic.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/UnionAPI.dll" "$ENV{COPY_DLL_TARGET}/Autorun/UnionAPI.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/dependencies/bass/lib/bass.dll" "$ENV{COPY_DLL_TARGET}/bass.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/dependencies/bass/lib/bassmidi.dll" "$ENV{COPY_DLL_TARGET}/bassmidi.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/dependencies/bass/lib/bassopus.dll" "$ENV{COPY_DLL_TARGET}/bassopus.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/dependencies/bass/lib/bassflac.dll" "$ENV{COPY_DLL_TARGET}/bassflac.dll"
            DEPENDS zbassmusic
            COMMENT "Copy zbassmusic to target directory: $ENV{COPY_DLL_TARGET}")
endif ()