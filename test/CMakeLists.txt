
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(
        charon_test
        main.cpp
        Settings.cpp
)

target_link_libraries(charon_test nlohmann_json::nlohmann_json)
target_link_libraries(charon_test Catch2::Catch2)
target_link_libraries(charon_test libcharon)

catch_discover_tests(charon_test)
