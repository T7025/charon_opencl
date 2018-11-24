//
// Created by thomas on 11/14/18.
//

#pragma once

#include <base/Util.hpp>
#include <base/Universe.hpp>

template<>
class Universe<Algorithm::bruteForce, Platform::cpuSingleThread, typename FP> {
public:
    Universe();

    void calculateNextStep();

};

