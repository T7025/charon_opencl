//#include <omp.h>
//#include <CL/cl.hpp>
#include <iostream>
#include <settings/Settings.hpp>
#include <barnes-hut/cpu-single-thread/Universe.hpp>
#include <BodyGenerators/SphereBodyGenerator.hpp>
#include <fstream>
#include <filesystem>
#include <simulator/Simulator.hpp>

//SCENARIO("settings parsing", "[settings]") {
//void SCENARIO() {
//    GIVEN("A ")
//}

int main() {
//    std::ios_base::sync_with_stdio(false);

    std::string settingsFilePrefix{"config"};
    settingsFilePrefix += '/';
    std::string settingsFile{"config.json"};
    Settings settings(settingsFilePrefix, settingsFile);

    Universe<Algorithm::bruteForce, Platform::cpuSingleThread, double> universe{settings};

    universe.init(std::make_unique<SphereBodyGenerator>(settings));
    {
        if (!std::filesystem::exists(settings.resultsDir)) {
            std::filesystem::create_directory(settings.resultsDir);
        }
        std::ofstream out{settings.resultsDir + '/' + settings.resultsFilenamePrefix + "01.txt"};
        universe.logInternalState(out);
    }

    Simulator simulator{settings};
    simulator.setup();
/*
    nlohmann::json j;
    j["pi"] = 3.14;
    j["list"] = {1, 2, 3};
    std::cout << j << "\n";
    std::cout << "Max threads (OpenMP): " << omp_get_max_threads() << std::endl;


    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);

    if (all_platforms.empty()) {
        std::cout << " No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    std::cout << "Available OpenCL platform(s): " << all_platforms[0].getInfo<CL_PLATFORM_NAME>();
    for (unsigned i = 1; i < all_platforms.size(); ++i) {
        std::cout << ", " << all_platforms[i].getInfo<CL_PLATFORM_NAME>();
    }*/
}