add_library(plugin SHARED)
set_target_properties(plugin PROPERTIES
        OUTPUT_NAME "zBassMusic"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}")

file(GLOB_RECURSE PLUGIN_SOURCES "src/**.cpp")
target_sources(plugin PRIVATE ${PLUGIN_SOURCES})

target_compile_definitions(plugin PRIVATE __G1 __G2A)
target_include_directories(plugin PRIVATE "src/")
target_link_libraries(plugin PRIVATE union-api gothic-api bass)

install(FILES $<TARGET_RUNTIME_DLLS:plugin> "${CMAKE_BINARY_DIR}/zBassMusic.dll" TYPE BIN)
install(SCRIPT "${VDF_DIR}/vdf.cmake")
install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/vdf/GothicVDFS.exe /B \"${CMAKE_SOURCE_DIR}/vdf/script.vdfs\" 
	    WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}\")")