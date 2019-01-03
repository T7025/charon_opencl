#include <utility>

//
// Created by thomas on 11/14/18.
//

#pragma once

#include <base/Util.hpp>
#include <cstddef>
#include <memory>
#include <settings/Settings.hpp>
#include "BodyGenerator.hpp"


class UniverseBase {
public:
    explicit UniverseBase(Settings settings) : settings{std::move(settings)} {};

    virtual ~UniverseBase() = default;

    /**
     * Initializes the bodies (stars, planets, particles, ...) in the universe.
     */
    virtual void init(std::unique_ptr<BodyGenerator> bodyGenerator) = 0;

    virtual void logInternalState(std::ostream &out) = 0;

    /**
     * Only used for testing.
     */
    std::vector<std::tuple<fp, Vec3<fp>, Vec3<fp>, Vec3<fp>>> getInternalState();

    virtual void step(unsigned int numSteps) = 0;

    /**
     * Useful for benchmarking OpenCL code.
     */
    virtual void finish() {};

protected:
    Settings settings;
    bool doneFirstStep = false;
};

template <enum Algorithm algorithm, enum Platform platform, typename FP>
class Universe : public UniverseBase {
public:
    explicit Universe(Settings settings) : UniverseBase{std::move(settings)} {};
};
