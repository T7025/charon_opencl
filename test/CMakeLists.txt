
add_executable(
        charon_test
        main.cpp
)
target_link_libraries(charon_test Catch2::Catch2)
catch_discover_tests(charon_test)
