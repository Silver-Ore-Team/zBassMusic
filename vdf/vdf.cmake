file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/vdf/System/Autorun")
file(COPY "${CMAKE_INSTALL_PREFIX}/bin/UnionAPI.dll" DESTINATION "${CMAKE_INSTALL_PREFIX}/vdf/System/Autorun")
file(COPY "${CMAKE_INSTALL_PREFIX}/bin/zBassMusic.dll" DESTINATION "${CMAKE_INSTALL_PREFIX}/vdf/System/Autorun")
file(COPY "${CMAKE_INSTALL_PREFIX}/bin/bass.dll" DESTINATION "${CMAKE_INSTALL_PREFIX}/vdf/System/Autorun")
