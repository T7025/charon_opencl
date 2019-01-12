//
// Created by thomas on 12/01/19.
//

#include "SFCIndex.hpp"

std::ostream &operator<<(std::ostream &out, const SFCIndex &sfcIndex) {
    out << std::bitset<k>{sfcIndex.x} << "," << std::bitset<k>{sfcIndex.y} << "," << std::bitset<k>{sfcIndex.z};
    return out;
}