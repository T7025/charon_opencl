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

    addSettings("algorithm", "brute-force");//, "barnes-hut");
    addSettings("floatingPointType", "float", "double");
    addSettings("numThreads", 6);
    addSettings("openclCompileOpts", "-cl-fast-relaxed-math");
//    addSettings("platform", "cpu-single-thread", "cpu-multi-thread", "opencl", "openclloc", "openclvec");
    addSettings("platform", "opencl", "openclloc", "openclvec");
    addSettings("timeStep", 0.001);
    addSettings("softeningLength", 0.005);
    addSettings("barnesHutCutoff", 0.7);
    addSettings("numberOfBodies", /*128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768,*/ 65536, 131072, 262144);

    return settings;
}

using std::chrono::steady_clock;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::seconds;
using namespace std::chrono_literals;


int main() {
    std::vector<nlohmann::json> results;

    typedef std::chrono::duration<double, std::nano> NS;
    NS ns = typename steady_clock::duration(1);
    std::cout << "Time resolution: " << ns.count() << "ns\n";

    //omp_set_num_threads(6);
    // Create standard json settings (with lib)
    // For each implementation: run with same settings
    // Put results in timestamped files/folders
    // Benchmark init, copy (= read, logInternalState) and compute separately.
    // Important metrics: num bodies, run time for each algorithm, device, FP
//    std::cout << "vector of floats: " << sizeof(std::vector<float>{}) << " bytes\n";
//    std::cout << "vector of doubles: " << sizeof(std::vector<double>{}) << " bytes\n";



    auto settings = getSettings();
    for (const auto &s : settings) {
        Settings setting{s};
        auto beforeInit = steady_clock::now();
        std::unique_ptr<UniverseBase> universe;
        try {
            universe = getConcreteUniverse(setting);
        }
        catch (NotImplementedUniverseException &e) {
            continue;
        }
        universe->init(std::make_unique<SphereBodyGenerator>(setting));
        universe->finish();
        auto afterInit = steady_clock::now();
        auto beforeSingleStep = steady_clock::now();
        universe->step(1);
        universe->finish();
        auto afterSingleStep = steady_clock::now();
        duration<double> expectedStepTime = afterSingleStep - beforeSingleStep;
        duration<double> requestedDuration = 5s;
        int minNumIterations = 10;

        int requestedNumIterations = (int) std::max(std::round(requestedDuration.count() / expectedStepTime.count()),
                                                    (double) minNumIterations);
        requestedNumIterations = (int) std::min((double)requestedNumIterations, 1000.0);

        std::cout << "Settings: " << s << "\n";
        std::cout << "Expected step time: " << expectedStepTime.count() << "s\n";
        std::cout << "Requested # iterations: " << requestedNumIterations << "\n";
        auto beforeSteps = steady_clock::now();
        int numIterations = 0;
        while (numIterations < requestedNumIterations && (numIterations < minNumIterations || duration_cast<seconds>(
                steady_clock::now() - beforeSteps).count() < 10)) {
            universe->step(1);
            universe->finish();
            ++numIterations;
//            if (numIterations > requestedNumIterations || (numIterations > minNumIterations && duration_cast<seconds>(
//                    steady_clock::now() - beforeSteps).count() > 10))
        }
        auto afterSteps = steady_clock::now();

        auto initDuration = duration_cast<nanoseconds>(afterInit - beforeInit);
        auto stepDuration = duration_cast<nanoseconds>(afterSteps - beforeSteps) / numIterations;
        auto totalDuration = initDuration + (afterSteps - beforeSteps);

        std::cout << "Total benchmark duration: " << duration<double>(totalDuration).count() << "s\n";
        std::cout << "Init duration: " << duration<double>(initDuration).count() << "s\n";
        std::cout << "Average step duration after " << numIterations << " steps: " << stepDuration.count()
                  << "ns (= " << duration<double>(stepDuration).count() << "s)\n";
        std::cout << std::endl;
        nlohmann::json resultJson{};
        resultJson["settings"] = s;
        resultJson["totalDuration"] = totalDuration.count();
        resultJson["initDuration"] = initDuration.count();
        resultJson["stepDuration"] = stepDuration.count();
        resultJson["numIterations"] = numIterations;
        results.emplace_back(resultJson);
    }

    std::string resultsDir = "benchResults";
    if (!std::filesystem::exists(resultsDir)) {
        std::filesystem::create_directory(resultsDir);
    }
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d-%H%M%S");
    auto outputFile = resultsDir + '/' + ss.str();
    if (std::ofstream out{outputFile + ".json"}; out.is_open()) {
        nlohmann::json result{};
        result["benchmarks"] = results;
        result["timeResolutionInNs"] = ns.count();
        out << result.dump(4) << "\n";
    }
    else {
        throw std::runtime_error{"could not write to file"};
    }

}