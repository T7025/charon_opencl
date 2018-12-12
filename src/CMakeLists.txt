#include(buildExample.cmake)

include_directories(.)

find_package(OpenCL 1.2 REQUIRED)
find_package(OpenMP REQUIRED)

add_library(
        libcharon
        base/Util.hpp
        settings/Settings.cpp
        settings/Settings.hpp
        simulator/Simulator.cpp
        simulator/Simulator.hpp
        base/Vec3.hpp
        base/Universe.hpp
        brute-force/cpu-single-thread/Universe.cpp
        brute-force/cpu-single-thread/Universe.hpp
        brute-force/cpu-multi-thread/Universe.cpp
        brute-force/cpu-multi-thread/Universe.hpp
        brute-force/opencl/Universe.cpp
        brute-force/opencl/Universe.hpp
        base/BodyGenerator.hpp
        BodyGenerators/SphereBodyGenerator.cpp
        BodyGenerators/SphereBodyGenerator.hpp
        BodyGenerators/BinaryBodyGenerator.cpp BodyGenerators/BinaryBodyGenerator.hpp base/getConcreteUniverse.cpp base/getConcreteUniverse.hpp base/Exceptions.cpp base/Exceptions.hpp base/ConcreteUniverseInclude.hpp)
target_compile_features(libcharon PUBLIC cxx_std_17)

target_compile_options(
        libcharon PUBLIC
        -Wall -Wextra
#        "$<$<CONFIG:DEBUG>:" -Og ">"
        # -fopt-info-vec-optimized/missed/note/all
        "$<$<CONFIG:RELEASE>:" -O3 -march=native -funroll-loops -ftree-vectorize -fopt-info-vec-all=vecreport.txt ">"
)

target_compile_definitions(
        libcharon PUBLIC
        CL_HPP_MINIMUM_OPENCL_VERSION=120  # Minimum OpenCL version: 1.2.0
        CL_HPP_TARGET_OPENCL_VERSION=120  # Target OpenCL version
        CL_HPP_ENABLE_EXCEPTIONS  # Enables exceptions for use in the C++ bindings header.
        $<$<BOOL:CHARON_USE_DOUBLE>: CHARON_USE_DOUBLE >
)

target_link_libraries(libcharon OpenCL::OpenCL)
target_link_libraries(libcharon pthread)
target_link_libraries(libcharon OpenMP::OpenMP_CXX)
target_link_libraries(libcharon nlohmann_json::nlohmann_json)
target_link_libraries(libcharon stdc++fs)

add_executable(
        charon
        main.cpp
)

target_link_libraries(charon libcharon)

get_target_property(charon_compile_options libcharon COMPILE_OPTIONS)
message("-- Charon compile options: ${charon_compile_options}")

get_target_property(charon_compile_definitions libcharon COMPILE_DEFINITIONS)
message("-- Charon compile definitions: ${charon_compile_definitions}")

get_target_property(charon_link_libraries libcharon LINK_LIBRARIES)
message("-- Charon link libraries: ${charon_link_libraries}")
