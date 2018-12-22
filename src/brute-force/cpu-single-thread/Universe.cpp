//
// Created by thomas on 11/14/18.
//

#include <base/Universe.hpp>

#include "Universe.hpp"

std::vector<std::tuple<fp, Vec3<fp>, Vec3<fp>, Vec3<fp>>> UniverseBase::getInternalState() {
    std::vector<std::tuple<fp, Vec3<fp>, Vec3<fp>, Vec3<fp>>> result;

    std::stringstream ss;
    logInternalState(ss);

    std::string line;
    std::getline(ss, line);  // Eat header
    while (ss) {
        auto getFp = [&]() {
            fp x;
            if (ss >> x)
                return x;
            return std::numeric_limits<fp>::quiet_NaN();
        };
        fp mass = getFp();
        if (mass != mass) {
            break;
        }
        Vec3<fp> pos{getFp(), getFp(), getFp()};
        Vec3<fp> vel{getFp(), getFp(), getFp()};
        Vec3<fp> acc{getFp(), getFp(), getFp()};
        result.emplace_back(mass, pos, vel, acc);
    }
    return result;
}
