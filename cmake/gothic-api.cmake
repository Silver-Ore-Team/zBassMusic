set(GOTHIC_API_WORKING_DIR "${CMAKE_CURRENT_BINARY_DIR}/gothic-api-working-dir")

file(MAKE_DIRECTORY ${GOTHIC_API_WORKING_DIR})
file(COPY "${GOTHIC_API_DIR}/" DESTINATION ${GOTHIC_API_WORKING_DIR})
file(COPY "${GOTHIC_USERAPI_DIR}/" DESTINATION "${GOTHIC_API_WORKING_DIR}/ZenGin/Gothic_UserAPI")

add_library(gothic-api INTERFACE IMPORTED)
target_include_directories(gothic-api INTERFACE "${GOTHIC_API_WORKING_DIR}")
target_link_directories(gothic-api INTERFACE "${GOTHIC_API_WORKING_DIR}")
target_sources(gothic-api INTERFACE "${GOTHIC_API_WORKING_DIR}/ZenGin/zGothicAPI.cpp")