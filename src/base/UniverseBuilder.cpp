//
// Created by thomas on 12/11/18.
//

#include "UniverseBuilder.hpp"

#include <sstream>
#include "Universe.hpp"

// Adapted from the example at https://en.cppreference.com/w/cpp/utility/integer_sequence
template <class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple_impl(std::basic_ostream<Ch, Tr> &os, const Tuple &t, std::index_sequence<Is...>) {
    ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template <class Ch, class Tr, class... Args>
auto &operator<<(std::basic_ostream<Ch, Tr> &os, const std::tuple<Args...> &t) {
    print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
    return os;
}


std::string createErrorString(const std::string &optionName, const std::string &optionValue,
                              const std::string &expectedOptions) {
    std::stringstream ss;
    ss << "Option '" << optionName << "' has invalid value '" << optionValue
       << "'. Expected one of " << expectedOptions << ".";
    return ss.str();
}

template <typename T> struct Entity { const char *name; const T value; };
template <typename FP> struct FPType { const char *name; typedef FP value_type; };

template <typename T>
auto &operator<<(std::ostream &os, const Entity<T> &entity) {
    os << "'" << entity.name << "'";
    return os;
}

template <typename FP>
auto &operator<<(std::ostream &os, const FPType<FP> &fpType) {
    os << "'" << fpType.name << "'";
    return os;
}

template <int algorithmIndex = 0, int platformIndex = 0, int fpIndex = 0>
std::unique_ptr<UniverseBase> getConcreteUniverse(const Settings &settings) {
    // Edit option values here:
    constexpr Entity<Algorithm> algorithmOptionsMap[] = {
            {"brute-force", Algorithm::bruteForce},
            {"barnes-hut",  Algorithm::barnesHut},
    };
    constexpr Entity<Platform> platformOptionsMap[] = {
            {"cpu-single-thread", Platform::cpuSingleThread},
            {"cpu-multi-thread",  Platform::cpuMultiThread},
            {"opencl", Platform::openCL}
    };
    constexpr auto fpTypeOptions = std::make_tuple(
            FPType<double>{"double"},
            FPType<float>{"float"}
    );

    constexpr int numAlgorithms = sizeof(algorithmOptionsMap) / sizeof(*algorithmOptionsMap);
    constexpr int numPlatforms = sizeof(platformOptionsMap) / sizeof(*platformOptionsMap);

    if constexpr (numAlgorithms == algorithmIndex) {
        std::stringstream ss;
        for (int i = 0; i < numAlgorithms; ++i) {
            ss << (i == 0 ? "" : ",") << algorithmOptionsMap[i];
        }
        throw std::runtime_error{createErrorString("algorithm", settings.algorithm, ss.str())};
    }
    else if constexpr (numPlatforms == platformIndex) {
        std::stringstream ss;
        for (int i = 0; i < numPlatforms; ++i) {
            ss << (i == 0 ? "" : ",") << platformOptionsMap[i];
        }
        throw std::runtime_error{createErrorString("platform", settings.platform, ss.str())};
    }
    else if constexpr (std::tuple_size<decltype(fpTypeOptions)>::value == fpIndex) {
        std::stringstream ss;
        ss << fpTypeOptions;
        throw std::runtime_error{createErrorString("floatingPointType", settings.floatingPointType, ss.str())};
    }
    else if (settings.algorithm != algorithmOptionsMap[algorithmIndex].name) {
        return getConcreteUniverse<algorithmIndex + 1, platformIndex, fpIndex>(settings);
    }
    else if (settings.platform != platformOptionsMap[platformIndex].name) {
        return getConcreteUniverse<algorithmIndex, platformIndex + 1, fpIndex>(settings);
    }
    else if (settings.floatingPointType != std::get<fpIndex>(fpTypeOptions).name) {
        return getConcreteUniverse<algorithmIndex, platformIndex, fpIndex + 1>(settings);
    }
    else {
        using UniverseType = Universe<algorithmOptionsMap[algorithmIndex].value,
                                      platformOptionsMap[platformIndex].value,
                                      typename std::tuple_element<fpIndex, decltype(fpTypeOptions)>::type::value_type>;
        if constexpr(std::is_abstract<UniverseType>::value) {
            throw std::runtime_error{
                    "The Universe with options:\n"
                    "\talgorithm = '" + settings.algorithm + "',\n" +
                    "\tplatform = '" + settings.platform + "',\n" +
                    "\tfloatingPointType = '" + settings.floatingPointType + "'\n" +
                    "is not implemented."};
        }
        else {
            return std::make_unique<UniverseType>(settings);
        }
    }
}

std::unique_ptr<UniverseBase> UniverseBuilder::operator()(const Settings &settings) {
    return getConcreteUniverse(settings);
}
