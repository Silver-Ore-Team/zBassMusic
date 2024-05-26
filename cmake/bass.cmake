add_library(bass SHARED IMPORTED)
set_property(TARGET bass PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${BASS_DIR}/include")
set_property(TARGET bass PROPERTY IMPORTED_LOCATION "${BASS_DIR}/lib/bass.dll")
set_property(TARGET bass PROPERTY IMPORTED_IMPLIB "${BASS_DIR}/lib/bass.lib")

add_library(bassmidi SHARED IMPORTED)
set_property(TARGET bassmidi PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${BASS_DIR}/include")
set_property(TARGET bassmidi PROPERTY IMPORTED_LOCATION "${BASS_DIR}/lib/bassmidi.dll")
set_property(TARGET bassmidi PROPERTY IMPORTED_IMPLIB "${BASS_DIR}/lib/bassmidi.lib")

add_library(bassopus SHARED IMPORTED)
set_property(TARGET bassopus PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${BASS_DIR}/include")
set_property(TARGET bassopus PROPERTY IMPORTED_LOCATION "${BASS_DIR}/lib/bassopus.dll")
set_property(TARGET bassopus PROPERTY IMPORTED_IMPLIB "${BASS_DIR}/lib/bassopus.lib")

add_library(bassflac SHARED IMPORTED)
set_property(TARGET bassflac PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${BASS_DIR}/include")
set_property(TARGET bassflac PROPERTY IMPORTED_LOCATION "${BASS_DIR}/lib/bassflac.dll")
set_property(TARGET bassflac PROPERTY IMPORTED_IMPLIB "${BASS_DIR}/lib/bassflac.lib")

add_library(bass_all INTERFACE)
target_link_libraries(bass_all INTERFACE bass bassmidi bassopus bassflac)