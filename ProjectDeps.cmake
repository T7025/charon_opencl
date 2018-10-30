
include(FetchContent)

# Dependencies
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.4.1
)
FetchContent_GetProperties(Catch2)
if (NOT Catch2_POPULATED)
    FetchContent_Populate(Catch2)
    set(CATCH_BUILD_TESTING OFF CACHE BOOL "Build SelfTest project")
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
endif ()

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.3.0
)
FetchContent_GetProperties(nlohmann_json)
if (NOT nlohmann_json_POPULATED)
    FetchContent_Populate(nlohmann_json)
    set(JSON_BuildTests OFF CACHE BOOL "Build the unit tests when BUILD_TESTING is enabled.")
    add_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR})
endif ()

#FetchContent_Declare(
#        Zupply
#        GIT_REPOSITORY https://github.com/zhreshold/zupply.git
#)
#FetchContent_GetProperties(Zupply)
#if (NOT Zupply_POPULATED)
#    FetchContent_Populate(Zupply)
#    add_subdirectory(${zupply_SOURCE_DIR}/build ${zupply_BINARY_DIR})
#endif ()
