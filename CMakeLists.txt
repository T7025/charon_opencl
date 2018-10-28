cmake_minimum_required(VERSION 3.12)

include(ProjectDeps.cmake)

project(charon_opencl
        VERSION 0.9
        LANGUAGES CXX)

add_subdirectory(src)
#add_subdirectory(test)
