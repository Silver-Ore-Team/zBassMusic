add_library(plugin SHARED)
set_target_properties(plugin PROPERTIES
        OUTPUT_NAME "zBassMusic"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/version.rc" @ONLY)
configure_file("${CMAKE_SOURCE_DIR}/vdf/script.vdfs.in" "${CMAKE_BINARY_DIR}/script.vdfs")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/NH.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB_RECURSE PLUGIN_SOURCES "src/**.cpp")
target_sources(plugin PRIVATE ${PLUGIN_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/version.rc")

target_compile_definitions(plugin PRIVATE _UNION_API_DLL __G1 __G2A)
target_include_directories(plugin PRIVATE "src/")
target_link_libraries(plugin PRIVATE union-api gothic-api bass)

install(FILES $<TARGET_RUNTIME_DLLS:plugin> "${CMAKE_BINARY_DIR}/zBassMusic.dll" TYPE BIN)
install(FILES "${CMAKE_BINARY_DIR}/script.vdfs" DESTINATION "${CMAKE_INSTALL_PREFIX}")
install(SCRIPT "${VDF_DIR}/vdf.cmake")
install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/vdf/GothicVDFS.exe /B \"${CMAKE_INSTALL_PREFIX}/script.vdfs\" 
	    WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}\")")