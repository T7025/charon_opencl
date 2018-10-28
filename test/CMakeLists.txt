
include(CTest)

add_executable(
        runUnitTests
        main.cpp
        #        BruteForceUniverseTest.cpp
        #        BarnesHutUniverseTest.cpp
)

target_link_libraries(runUnitTests libcharon gtest gmock gtest_main)
add_test(NAME runUnitTests COMMAND runUnitTests)
