//
// Created by thomas on 12/4/18.
//
#include <nlohmann/json.hpp>
#include <settings/Settings.hpp>
#include <base/BodyGenerator.hpp>
#include <base/Universe.hpp>
#include <base/getConcreteUniverse.hpp>
#include <base/Exceptions.hpp>
#include <iostream>
#include <BodyGenerators/BinaryBodyGenerator.hpp>
#include <BodyGenerators/SphereBodyGenerator.hpp>
#include <chrono>
#include <filesystem>
#include <omp.h>
#include <fstream>

template <typename FP>
std::vector<unsigned> getInterestingNumberOfBodies() {
    // lstopo (or lstopo-no-graphics) for overview
    // "getconf -a | grep CACHE" for cache sizes in bytes
    // Laptop:
    //   L1d: 32K
    //   L2: 256K
    //   L3: 3072K
    // Desktop:
    //   L1d: 32K
    //   L2: 4096K
    //   L3: 16384K
    // Studento:
    //   L1d: 16K
    //   L2: 2048K
    //   L3: 6144K
    // Fire:
    //   L1d: 32K
    //   L2: 256K
    //   L3: 30720K

    // 1 body in memory: (1 + 3 + 3 + 3) * sizeof(FP)
    // sizeof(FP) == 32 -> 10 * 32 bit/body -> 40 byte/body
    // sizeof(FP) == 64 -> 10 * 64 bit/body -> 80 byte/body

    // Cache             # bodies
    // KB    byte        float       double
    //    16       16384       409.6      204.8
    //    32       32768       819.2      409.6
    //   256      262144      6553.6     3276.8
    //  2048     2097152     52428.8    26214.4
    //  3072     3145728     78643.2    39321.6
    //  4096     4194304    104857.6    52428.8
    //  6144     6291456    157286.4    78643.2
    // 16384    16777216    419430.4   209715.2
    // 30720    31457280    786432     393216
    // Assumes that the Universe/Vector/... classes are 0 bytes.

    return {};
}

std::vector<nlohmann::json> getSettings() {

    std::vector<nlohmann::json> settings{nlohmann::json{}};

    auto addSettings = [&settings](const std::string &name, auto ...values) {
        auto addSettingsImpl = [&settings](auto &self, size_t size, const std::string &name, auto &&value,
                                           auto &&...values) {
            if constexpr(sizeof...(values) > 0) {
                self(self, size, name, values...);
            }
            for (unsigned i = 0; i < size; ++i) {
                auto tempSettings = settings[i];
                tempSettings[name] = value;
                settings[i + size * sizeof...(values)] = tempSettings;
            }
        };
        const auto size = settings.size();
        settings.resize(settings.size() * sizeof...(values));
        addSettingsImpl(addSettingsImpl, size, name, values...);
    };

    addSettings("algorithm", "brute-force", "barnes-hut");
    addSettings("floatingPointType", "float", "double");
    addSettings("platform", "cpu-single-thread", "cpu-multi-thread", "opencl");
    addSettings("barnesHutCutoff", 0.7);
    addSettings("numberOfBodies", 128, 256, 512, 1024, 2048, 4096);


//    for (const auto &s : settings) {
//        std::cout << s << "\n";
//    }

    return settings;
}


class NullBuffer : public std::streambuf {
public:
    int overflow(int c) { return c; }
};

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;

int main() {

    omp_set_num_threads(6);
    // Create standard json settings (with lib)
    // For each implementation: run with same settings
    // Put results in timestamped files/folders
    // Benchmark init, copy (= read, logInternalState) and compute separately.
    // Important metrics: num bodies, run time for each algorithm, device, FP
//    std::cout << "vector of floats: " << sizeof(std::vector<float>{}) << " bytes\n";
//    std::cout << "vector of doubles: " << sizeof(std::vector<double>{}) << " bytes\n";

    std::string resultsDir = "benchResults";
    if (!std::filesystem::exists(resultsDir)) {
        std::filesystem::create_directory(resultsDir);
    }
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d-%H%M%S");
    auto outputDir = resultsDir + '/' + ss.str();
    std::filesystem::create_directory(outputDir);


    std::vector<nlohmann::json> results;

    auto settings = getSettings();
    for (const auto &s : settings) {
        Settings setting{s};
        std::unique_ptr<UniverseBase> universe;
        try {
            universe = getConcreteUniverse(setting);
        }
        catch (NotImplementedUniverseException &e) {
            continue;
        }
        universe->init(std::make_unique<SphereBodyGenerator>(setting));
        universe->step(1);
        universe->finish();

        auto start = steady_clock::now();
        int numIterations = 0;
        while (numIterations < 30 &&
               (duration_cast<seconds>(steady_clock::now() - start).count() < 10 || numIterations > 10)) {
            universe->step(1);
            ++numIterations;
        }

        universe->finish();
//        NullBuffer nullBuffer{};
//        std::ostream nullStream{&nullBuffer};
//        universe->logInternalState(nullStream);  // Make sure queue gets flushed when using OpenCL

        auto end = steady_clock::now();
        auto diff = duration_cast<milliseconds>(end - start);
        std::cout << s << "\n";
        std::cout << "Average duration after " << numIterations << " steps: " << diff.count() / numIterations << "ms"
                  << "\n";
        nlohmann::json resultJson{};
        resultJson["settings"] = s;
        resultJson["numIterations"] = numIterations;
        resultJson["stepDuration"] = diff.count() / numIterations;
        std::cout << "resultJson: " << resultJson << "\n";
        results.emplace_back(resultJson);
    }

    if (std::ofstream out{outputDir + ".json"}; out.is_open()) {
        out << nlohmann::json{results} << "\n";
    }
    else {
        throw std::runtime_error{"could not write to file"};
    }

}