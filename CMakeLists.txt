cmake_minimum_required(VERSION 3.12)

# Options
option(CHARON_BUILD_TESTING "Build the unit tests when BUILD_TESTING is enabled." ON)
option(CHARON_USE_DOUBLE "Use double precision floating point." ON)
option(CHARON_TRAVIS_TEST "Testing with Travis CL." OFF)

project(charon_opencl
        VERSION 0.9
        LANGUAGES CXX)

include(ProjectDeps.cmake)


add_subdirectory(src)


include(CTest) #adds option BUILD_TESTING (default ON)

if(BUILD_TESTING AND CHARON_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(bencmark)

#add_subdirectory(test)
