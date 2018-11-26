
//
// Created by thomas on 01/11/18.
//

#include "Simulator.hpp"
#include <utility>
//#include <base/Universe.hpp>
#include <barnes-hut/cpu-single-thread/Universe.hpp>
#include <iostream>

template <enum Algorithm algorithm, enum Platform platform, typename FP>
std::unique_ptr<UniverseBase> getConcreteUniverse(const Settings &settings) {
    if constexpr(std::is_abstract<Universe<algorithm, platform, FP>>::value) {
        throw std::runtime_error{
                "The Universe with options\n"
                "\talgorithm = '" + settings.algorithm + "',\n" +
                "\tplatform = '" + settings.platform + "',\n" +
                "\tfloatingPointType = '" + settings.floatingPointType + "'\n" +
                "is not implemented."};
    } else {
        return std::make_unique<Universe<algorithm, platform, FP>>(settings);
    }
}

template <enum Algorithm algorithm, enum Platform platform>
std::unique_ptr<UniverseBase> getConcreteUniverse(const Settings &settings) {
    if (settings.floatingPointType == "double") {
        return getConcreteUniverse<algorithm, platform, double>(settings);
    } else if (settings.floatingPointType == "float") {
        return getConcreteUniverse<algorithm, platform, float>(settings);
    } else {
        throw std::runtime_error{"Option 'floatingPointType' has invalid value '"
                                 + settings.algorithm
                                 + "'. Expected 'double' or 'float'."};
    }
}

template <enum Algorithm algorithm>
std::unique_ptr<UniverseBase> getConcreteUniverse(const Settings &settings) {
    if (settings.platform == "cpu-single-thread") {
        return getConcreteUniverse<algorithm, Platform::cpuSingleThread>(settings);
    } else if (settings.platform == "cpu-multi-thread") {
        return getConcreteUniverse<algorithm, Platform::cpuMultiThread>(settings);
    } else {
        throw std::runtime_error{"Option 'platform' has invalid value '"
                                 + settings.algorithm
                                 + "'. Expected 'cpu-single-thread' or 'cpu-multi-thread'."};
    }
}

std::unique_ptr<UniverseBase> getConcreteUniverse(const Settings &settings) {
    if (settings.algorithm == "brute-force") {
        return getConcreteUniverse<Algorithm::bruteForce>(settings);
    } else if (settings.algorithm == "barnes-hut") {
        return getConcreteUniverse<Algorithm::barnesHut>(settings);
    } else {
        throw std::runtime_error{"Option 'algorithm' has invalid value '"
                                 + settings.algorithm
                                 + "'. Expected 'brute-force' or 'barnes-hut'."};
    }
}

Simulator::Simulator(Settings settings) : settings{std::move(settings)}, universe{getConcreteUniverse(settings)} {}

void Simulator::setup() {
    std::cout << "test" << "\n";
//    universe = getConcreteUniverse<Algorithm::bruteForce, Platform::cpuSingleThread>(settings);
//    universe = getConcreteUniverse(settings);
//    auto getConcreteUniverse = [] <
//    enum Algorithm algorithm>()
//    {
//
//    };
}
