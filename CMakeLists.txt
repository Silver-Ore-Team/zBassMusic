cmake_minimum_required(VERSION 3.25)
set(PROJECT_VERSION "0.1.3")
set(PROJECT_VERSION_CMAKE "0.3.4")
project(zBassMusic VERSION ${PROJECT_VERSION_CMAKE})

option(BUILD_VDF "Build .VDF file with plugin" ON)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BASS_DIR "${CMAKE_SOURCE_DIR}/dependencies/bass")
set(VDF_DIR "${CMAKE_SOURCE_DIR}/vdf")
set(GOTHIC_USERAPI_DIR "${CMAKE_SOURCE_DIR}/gothic-userapi")

set(CMAKE_CXX_STANDARD 23)
if (${CMAKE_BUILD_RELEASE_PDB})
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif ()

include(FetchContent)
set(GOTHIC_API_G1  ON)
set(GOTHIC_API_G1A ON)
set(GOTHIC_API_G2  ON)
set(GOTHIC_API_G2A ON)
FetchContent_Declare(
        union-api
        GIT_REPOSITORY https://gitlab.com/union-framework/union-api.git
        GIT_TAG        ede619be94cb6a7349c43b6678748ad1113a8c56
)
FetchContent_Declare(
        gothic-api
        GIT_REPOSITORY https://gitlab.com/union-framework/gothic-api.git
        GIT_TAG        ba0894b136eda163209ef58a7dad07103cb2ed10
)
FetchContent_MakeAvailable(union-api)
FetchContent_MakeAvailable(gothic-api)

include(cmake/bass.cmake)

add_library(plugin SHARED)
set_target_properties(plugin PROPERTIES
        OUTPUT_NAME "zBassMusic"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}")

string(TIMESTAMP BUILD_TIME "%Y-%m-%dT%H:%M:%S")
execute_process(COMMAND git symbolic-ref --short HEAD OUTPUT_VARIABLE BUILD_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git log --format=%H -n 1 OUTPUT_VARIABLE BUILD_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/version.rc" @ONLY)
configure_file("${CMAKE_SOURCE_DIR}/vdf/script.vdfs.in" "${CMAKE_BINARY_DIR}/script.vdfs")
configure_file("${CMAKE_SOURCE_DIR}/src/BuildInfo.h.in" "${CMAKE_BINARY_DIR}/src/BuildInfo.h" @ONLY)

file(GLOB_RECURSE PLUGIN_SOURCES "src/**.cpp")
target_sources(plugin PRIVATE ${PLUGIN_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/version.rc")

target_compile_options(plugin PRIVATE
        /W4 /WX /permissive- /Zc:__cplusplus /Zc:preprocessor /utf-8
        /experimental:external /external:W0 /external:anglebrackets /external:templates-
        /external:I ${CMAKE_BINARY_DIR}/_deps
        /wd4740 # disable warning C4740: flow in or out of inline asm code suppresses global optimization
)

target_include_directories(plugin PRIVATE "src/" "${CMAKE_BINARY_DIR}/src/")
target_link_libraries(plugin PRIVATE union_api_lib gothic_api bass_all)

install(FILES $<TARGET_RUNTIME_DLLS:plugin> "${CMAKE_BINARY_DIR}/zBassMusic.dll" TYPE BIN)
if (${BUILD_VDF})
    install(FILES "${CMAKE_BINARY_DIR}/script.vdfs" DESTINATION "${CMAKE_INSTALL_PREFIX}")
    install(SCRIPT "${VDF_DIR}/vdf.cmake")
    install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/vdf/GothicVDFS.exe /B \"${CMAKE_INSTALL_PREFIX}/script.vdfs\"
            WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}\")")
endif ()

if (${BUILD_VDF})
    string(REPLACE "build" "install" INSTALL_DIR "${CMAKE_BINARY_DIR}")
    add_custom_target(plugin_vdf ALL
            COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${INSTALL_DIR}
            DEPENDS plugin
            COMMENT "Run install to build VDF")

    if (DEFINED ENV{COPY_VDF_TARGET})
        add_custom_target(copy_vdf ALL
                COMMAND ${CMAKE_COMMAND} -E copy "${INSTALL_DIR}/zBassMusic.vdf" "$ENV{COPY_VDF_TARGET}"
                DEPENDS plugin_vdf
                COMMENT "Copy plugin to target directory: $ENV{COPY_VDF_TARGET}")
    endif ()
endif ()

if (DEFINED ENV{COPY_DLL_TARGET})
    add_custom_target(copy_dll ALL
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/zBassMusic.dll" "$ENV{COPY_DLL_TARGET}/Autorun/zBassMusic.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/dependencies/bass/lib/bass.dll" "$ENV{COPY_DLL_TARGET}/bass.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/dependencies/bass/lib/bassmidi.dll" "$ENV{COPY_DLL_TARGET}/bassmidi.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/dependencies/bass/lib/bassopus.dll" "$ENV{COPY_DLL_TARGET}/bassopus.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/dependencies/bass/lib/bassflac.dll" "$ENV{COPY_DLL_TARGET}/bassflac.dll"
            DEPENDS plugin
            COMMENT "Copy plugin to target directory: $ENV{COPY_DLL_TARGET}")
endif ()
