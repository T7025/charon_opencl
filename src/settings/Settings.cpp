//
// Created by thomas on 02/11/18.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Settings.hpp"

Settings::Settings(const std::string &string_settingsFilePrefix, const std::string &settingsFile) {

    std::ifstream input{string_settingsFilePrefix + settingsFile};
    if (input) {
        nlohmann::json json;
        input >> json;

        enableFileOutput = json.value("enableFileOutput", false);
        resultsDir = json.value("resultsDir", "results");
        resultsFilenamePrefix = json.value("resultsFilenamePrefix", "out_");
        nrOfSteps = json.value("nrOfSteps", 0);

        snapshotDelta = json.value("snapshotDelta", 0);
        algorithm = json.value("algorithm", "brute-force");
        platform = json.value("platform", "cpu-single-thread");
        floatingPointType = json.value("floatingPointType", "double");
        universeShape = json.value("universeShape", "");

        barnesHutCutoff = json.value("barnesHutCutoff", 0.7);

        rngSeed = json.value("rngSeed", 0);
        timeStep = json.value("timeStep", 0.001);

        numberOfBodies = json.value("numberOfBodies", 2u);

    }
    else {
        throw std::runtime_error{"File \"" + string_settingsFilePrefix + settingsFile + "\" not found."};
    }
}
