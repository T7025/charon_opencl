include_directories(../src)

add_executable(
        benchmark
        main.cpp
)
target_compile_features(benchmark PUBLIC cxx_std_17)

target_link_libraries(benchmark libcharon)
target_link_libraries(benchmark nlohmann_json::nlohmann_json)
target_link_libraries(benchmark stdc++fs)

