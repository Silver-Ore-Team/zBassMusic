cmake_minimum_required(VERSION 3.25..3.29.5)
set(PROJECT_VERSION "0.1.3")
set(PROJECT_VERSION_CMAKE "0.3.2")
set(VCPKG_TARGET_ARCHITECTURE "x86")
set(VCPKG_PLATFORM_TOOLSET "v143")
project(zBassMusic VERSION "${PROJECT_VERSION_CMAKE}" LANGUAGES CXX)

option(BUILD_VDF "Build .VDF file with plugin" ON)

set(UNION_API_DIR "${CMAKE_SOURCE_DIR}/dependencies/union-api")
set(GOTHIC_API_DIR "${CMAKE_SOURCE_DIR}/dependencies/gothic-api")
set(BASS_DIR "${CMAKE_SOURCE_DIR}/dependencies/bass")
set(VDF_DIR "${CMAKE_SOURCE_DIR}/vdf")
set(GOTHIC_USERAPI_DIR "${CMAKE_SOURCE_DIR}/gothic-userapi")

set(CMAKE_CXX_STANDARD 23)
if (${CMAKE_BUILD_RELEASE_PDB})
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif ()

find_package(spdlog CONFIG REQUIRED)

include(cmake/union-api.cmake)
include(cmake/gothic-api.cmake)
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

file(GLOB_RECURSE ZBASSMUSIC_SOURCES "src/**.cpp" "${UNION_API_DIR}/union-api/Union/Memory.cpp")
target_sources(zbassmusic PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/version.rc" ${ZBASSMUSIC_SOURCES})

add_compile_options(zbassmusic PRIVATE /W4 /WX)
target_compile_definitions(zbassmusic PRIVATE _UNION_API_DLL __G1 __G1A __G2 __G2A)
target_include_directories(zbassmusic PRIVATE "src/" "${CMAKE_BINARY_DIR}/src/")
target_link_libraries(zbassmusic PRIVATE union-api gothic-api bass_all)

install(FILES $<TARGET_RUNTIME_DLLS:zbassmusic> "${CMAKE_BINARY_DIR}/zBassMusic.dll" TYPE BIN)
install(FILES $<TARGET_RUNTIME_DLLS:zbassmusic> "${CMAKE_BINARY_DIR}/UnionAPI.dll" TYPE BIN)
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