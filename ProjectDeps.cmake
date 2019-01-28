
# Set FetchContent options
option(FETCHCONTENT_QUIET "Enable silent mode (default 'ON')" ON)
option(FETCHCONTENT_FULLY_DISCONNECTED "Don't attempt to download or update any content (default 'OFF')" OFF)

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
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${catch2_SOURCE_DIR}/contrib/")
    include(Catch)
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
#        boost_compute
#        GIT_REPOSITORY https://github.com/boostorg/compute.git
#        GIT_TAG boost-1.69.0
#)
#FetchContent_GetProperties(boost_compute)
#if (NOT boost_compute_POPULATED)
#    FetchContent_Populate(boost_compute)
#    set(BOOST_COMPUTE_USE_CPP11 ON CACHE BOOL "Use C++11 features")
#    add_subdirectory(${boost_compute_SOURCE_DIR} ${boost_compute_BINARY_DIR})
#
#endif ()

#FetchContent_Declare(
#        Zupply
#        GIT_REPOSITORY https://github.com/zhreshold/zupply.git
#)
#FetchContent_GetProperties(Zupply)
#if (NOT Zupply_POPULATED)
#    FetchContent_Populate(Zupply)
#    add_subdirectory(${zupply_SOURCE_DIR}/build ${zupply_BINARY_DIR})
#endif ()
