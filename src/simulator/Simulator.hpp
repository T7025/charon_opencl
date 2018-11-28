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

    void run();

private:
    Settings settings;
    std::unique_ptr<UniverseBase> universe;
    std::string outputDir;

    void snapshot(unsigned int fileNr) const;
};
