add_library(union-api SHARED)
set_target_properties(union-api PROPERTIES
        OUTPUT_NAME "UnionAPI"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}")

target_include_directories(union-api PUBLIC "${UNION_API_DIR}/union-api")
target_link_directories(union-api PUBLIC "${UNION_API_DIR}/union-api")

file(GLOB_RECURSE UNION_SOURCES "${UNION_API_DIR}/union-api/union-api.cpp" "${UNION_API_DIR}/union-api/Union/Memory.cpp")
target_sources(union-api PRIVATE ${UNION_SOURCES})

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(union-api PUBLIC WIN32 _DEBUG _CONSOLE _UNION_API_DLL PRIVATE _UNION_API_BUILD)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    target_compile_definitions(union-api PUBLIC WIN32 NDEBUG _CONSOLE _UNION_API_DLL PRIVATE _UNION_API_BUILD)
else()
    message(FATAL_ERROR "Invalid $CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} not in range [Debug, Release]")
endif()