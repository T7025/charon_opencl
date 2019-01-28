
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(
        charon_test
        main.cpp
        SettingsTest.cpp
        UniverseTest.cpp)

target_compile_features(charon_test PUBLIC cxx_std_17)

if(CHARON_TRAVIS_TEST)
target_compile_definitions(
        charon_test PUBLIC
        CHARON_TRAVIS_TEST
)
endif()

target_link_libraries(charon_test nlohmann_json::nlohmann_json)
target_link_libraries(charon_test Catch2::Catch2)
target_link_libraries(charon_test libcharon)

catch_discover_tests(charon_test)

configure_file(testConfig.json testConfig.json)
