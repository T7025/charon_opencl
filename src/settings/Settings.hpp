//
// Created by thomas on 02/11/18.
//

#pragma once


#include <string>
#include <base/Util.hpp>

class Settings {
public:
    Settings(const std::string &string_settingsFilePrefix, const std::string &settingsFile);

    bool enableFileOutput;
    std::string resultsDir;
    std::string resultsFilenamePrefix;

    unsigned nrOfSteps;  ///< Total nr. of steps to simulate.
    unsigned snapshotDelta;  ///< Amount of steps between each snapshot of the universe.
    std::string algorithm;
    std::string platform;
    std::string floatingPointType; ///< float or double.
    std::string universeShape;  ///< Initial shape of the universe.

    fp barnesHutCutoff;  ///< When using a Barnes-Hut implementation.

    long rngSeed;
    fp timeStep;

    unsigned numberOfBodies;  ///< Number of bodies to simulate.

    fp universeRadius;  ///< Radius of the universe in km


};
