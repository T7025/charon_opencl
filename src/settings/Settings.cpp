//
// Created by thomas on 02/11/18.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Settings.hpp"

Settings::Settings(const nlohmann::json &json) :
        enableFileOutput{json.value("enableFileOutput", false)},
        resultsDir{json.value("resultsDir", "results")},
        resultsFilenamePrefix{json.value("resultsFilenamePrefix", "out_")},

        nrOfSteps{json.value("nrOfSteps", 0u)},
        snapshotDelta{json.value("snapshotDelta", 0u)},
        algorithm{json.value("algorithm", "brute-force")},
        platform{json.value("platform", "cpu-single-thread")},
        floatingPointType{json.value("floatingPointType", "double")},
        universeShape{json.value("universeShape", "")},

        barnesHutCutoff{json.value("barnesHutCutoff", fp(0.7))},

        rngSeed{json.value("rngSeed", 1302u)},
        timeStep{json.value("timeStep", fp(0.001))},

        numberOfBodies{json.value("numberOfBodies", 2u)},
//        numberOfBodies{2u},

        universeRadius{json.value("universeRadius", fp(1))} {}


Settings::Settings(const std::string &settingsFilePrefix, const std::string &settingsFile) :
        Settings{[&] {
            nlohmann::json json;
            std::ifstream input{settingsFilePrefix + settingsFile};
            if (input) {
                input >> json;
            } else {
                throw std::runtime_error{"File \"" + settingsFilePrefix + settingsFile + "\" not found."};
            }
            return json;
        }()} {}

