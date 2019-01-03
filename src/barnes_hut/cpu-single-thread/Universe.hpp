//
// Created by thomas on 01/01/19.
//

#pragma once

#include <base/Universe.hpp>

template <typename FP>
class Universe<Algorithm::barnesHut, Platform::cpuSingleThread, FP> : public UniverseBase {
public:
    using UniverseBase::UniverseBase;

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override {
        auto[m, pos, vel] = bodyGenerator->getBody();

    }
};

