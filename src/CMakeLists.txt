#include(buildExample.cmake)

include_directories(.)

add_library(
        libcharon
        base/Util.hpp
        base/Universe.hpp
        barnes-hut/cpu-single-thread/Universe.cpp
        barnes-hut/cpu-single-thread/Universe.hpp
        settings/Settings.cpp
        settings/Settings.hpp
        simulator/Simulator.cpp
        simulator/Simulator.hpp
        base/BodyGenerator.hpp
        base/Vec3.hpp
        BodyGenerators/CubeBodyGenerator.cpp
        BodyGenerators/CubeBodyGenerator.hpp
)
target_compile_features(libcharon PUBLIC cxx_std_20)

target_link_libraries(libcharon OpenCL::OpenCL)
target_link_libraries(libcharon OpenMP::OpenMP_CXX)
target_link_libraries(libcharon nlohmann_json::nlohmann_json)
target_link_libraries(libcharon pthread)

add_executable(
        charon
        main.cpp
        #        settings/Settings.cpp settings/Settings.hpp base/Util.hpp
)

target_compile_features(charon PUBLIC cxx_std_20)

target_compile_options(
        charon PUBLIC
        -Wall -Wextra
        "$<$<CONFIG:DEBUG>:" -Og ">"
        # -fopt-info-vec-optimized/missed/note/all
        "$<$<CONFIG:RELEASE>:" -O3 -march=native -funroll-loops -ftree-vectorize -fopt-info-vec-all=vecreport.txt ">"
)
target_compile_definitions(
        charon PUBLIC
        CL_HPP_MINIMUM_OPENCL_VERSION=200  # Minimum OpenCL version: 2.0.0
        CL_HPP_ENABLE_EXCEPTIONS  # Enables exceptions for use in the C++ bindings header.
        $<$<BOOL:CHARON_USE_DOUBLE>: CHARON_USE_DOUBLE >
)


find_package(OpenCL 1.2 REQUIRED)
find_package(OpenMP REQUIRED)

target_link_libraries(charon OpenCL::OpenCL)
target_link_libraries(charon OpenMP::OpenMP_CXX)
target_link_libraries(charon nlohmann_json::nlohmann_json)
target_link_libraries(charon pthread)

target_link_libraries(charon libcharon)

get_target_property(charon_compile_options charon COMPILE_OPTIONS)
message("-- Charon compile options: ${charon_compile_options}")

get_target_property(charon_compile_definitions charon COMPILE_DEFINITIONS)
message("-- Charon compile definitions: ${charon_compile_definitions}")

get_target_property(charon_link_libraries charon LINK_LIBRARIES)
message("-- Charon link libraries: ${charon_link_libraries}")
