
add_executable(
        charon
        main.cpp
)

target_compile_features(charon PUBLIC cxx_std_14)

target_compile_options(
        charon PUBLIC
        -Wall -Wextra
        "$<$<CONFIG:DEBUG>:-Og>"
        "$<$<CONFIG:RELEASE>:-O3 -march=native -funroll-loops -ftree-vectorize -fopt-info-vec-all=vecreport.txt>"
)
target_compile_definitions(
        charon PRIVATE
        CL_HPP_MINIMUM_OPENCL_VERSION=200  # Minimum OpenCL version: 2.0.0
        CL_HPP_ENABLE_EXCEPTIONS  # Enables exceptions for use in the C++ bindings header.
)


find_package(OpenCL 2.2 REQUIRED)
find_package(OpenMP REQUIRED)

target_link_libraries(charon OpenCL::OpenCL)
target_link_libraries(charon OpenMP::OpenMP_CXX)
target_link_libraries(charon nlohmann_json::nlohmann_json)
#target_link_libraries(charon pthread)


get_target_property(charon_compile_options charon COMPILE_OPTIONS)
message("-- Charon compile options: ${charon_compile_options}")

get_target_property(charon_compile_definitions charon COMPILE_DEFINITIONS)
message("-- Charon compile definitions: ${charon_compile_definitions}")

get_target_property(charon_link_libraries charon LINK_LIBRARIES)
message("-- Charon link libraries: ${charon_link_libraries}")