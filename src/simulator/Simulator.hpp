//
// Created by thomas on 01/11/18.
//

#pragma once


#include <memory>
#include <settings/Settings.hpp>

class UniverseBase;

class Simulator {
public:
    explicit Simulator(Settings settings);

    void setup();

    void run();

private:
    Settings settings;
    std::unique_ptr<UniverseBase> universe;
};
