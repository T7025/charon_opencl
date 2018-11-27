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


//template<enum Algorithm, enum Platform>
class UniverseBase {
public:
    explicit UniverseBase(Settings settings) : settings_{std::move(settings)} {};

    virtual ~UniverseBase() = default;

    /**
     * Initializes the bodies (stars, planets, particles, ...) in the universe.
     */
    virtual void init(std::unique_ptr<BodyGenerator> bodyGenerator) = 0;

    virtual void logInternalState(std::ostream &out) = 0;

    virtual void step() = 0;

protected:
    const Settings &settings() const { return settings_; }

private:
    Settings settings_;
};

template <enum Algorithm algorithm, enum Platform platform, typename FP>
class Universe : public UniverseBase {
public:
    explicit Universe(Settings settings) : UniverseBase{std::move(settings)} {};

//    template <typename BodyGen>
//    void init(std::unique_ptr<BodyGenerator> bodyGeneratorType) {};
//
//    virtual void logInternalState(std::ostream &out) = 0;
//
//    virtual void step() = 0;

};
