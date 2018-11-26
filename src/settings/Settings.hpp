//
// Created by thomas on 02/11/18.
//

#pragma once


#include <string>
#include <memory>
#include <base/Util.hpp>
#include <nlohmann/json.hpp>


class Settings {
public:
    Settings(const std::string &settingsFilePrefix, const std::string &settingsFile);
/*

    void init(const std::string &settingsFilePrefix, const std::string &settingsFile);
*/

    explicit Settings(const nlohmann::json &json);

    const bool enableFileOutput;
    const std::string resultsDir;
    const std::string resultsFilenamePrefix;

    const unsigned nrOfSteps;  ///< Total nr. of steps to simulate.
    const unsigned snapshotDelta;  ///< Amount of steps between each snapshot of the universe.
    const std::string algorithm;
    const std::string platform;
    const std::string floatingPointType; ///< float or double.
    const std::string universeShape;  ///< Initial shape of the universe.

    const fp barnesHutCutoff;  ///< When using a Barnes-Hut implementation.

    const unsigned long rngSeed;
    const fp timeStep;

    const unsigned numberOfBodies;  ///< Number of bodies to simulate.

    const fp universeRadius;  ///< Radius of the universe.

    unsigned getNr() { return numberOfBodies; }


};
