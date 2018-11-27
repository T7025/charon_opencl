//
// Created by thomas on 02/11/18.
//

#include <fstream>
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
        bodyGeneratorType{json.value("bodyGeneratorType", "sphere")},

        barnesHutCutoff{json.value("barnesHutCutoff", fp(0.7))},

        rngSeed{json.value("rngSeed", 1302u)},
        timeStep{json.value("timeStep", fp(0.001))},

        numberOfBodies{json.value("numberOfBodies", 2u)},

        universeRadius{json.value("universeRadius", fp(1))},
        totalMass{1.0},
        G{1.0} {}


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

