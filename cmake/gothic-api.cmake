add_library(gothic-api INTERFACE IMPORTED)
target_include_directories(gothic-api INTERFACE
        "${GOTHIC_USERAPI_DIR}"
        "${GOTHIC_API_DIR}"
        "${GOTHIC_API_DIR}/ZenGin/Gothic_UserAPI"
)
target_link_directories(gothic-api INTERFACE "${GOTHIC_API_DIR}")
target_sources(gothic-api INTERFACE "${GOTHIC_API_DIR}/ZenGin/zGothicAPI.cpp")