//
// Created by thomas on 01/11/18.
//

#pragma once


#include <memory>
#include <settings/Settings.hpp>

class Simulator {
public:
    Simulator(std::shared_ptr<Settings> settings);

    void setup();

    void run();

private:
    const unsigned nrOfSteps;
    const unsigned snapshotDelta;


};
