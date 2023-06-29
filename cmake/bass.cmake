add_library(bass SHARED IMPORTED)
set_property(TARGET bass PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${BASS_DIR}/include")
set_property(TARGET bass PROPERTY IMPORTED_LOCATION "${BASS_DIR}/lib/bass.dll")
set_property(TARGET bass PROPERTY IMPORTED_IMPLIB "${BASS_DIR}/lib/bass.lib")
