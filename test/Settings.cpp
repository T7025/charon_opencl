//
// Created by thomas on 11/26/18.
//

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>
#include <settings/Settings.hpp>


SCENARIO("Settings parsing from json object", "[settings]") {
    GIVEN("A json object containing parsable settings") {
        nlohmann::json json;
        json["resultsDir"] = "testRes";
        json["numberOfBodies"] = 500;

        WHEN("A Settings object is initialized with the json object") {
            Settings settings{json};
            THEN("The Settings object should reflect the configuration values in the json object") {
                REQUIRE(settings.resultsDir == json["resultsDir"]);
                REQUIRE(settings.numberOfBodies == json["numberOfBodies"]);
            }
        }
    }
}

SCENARIO("Settings parsing from json file", "[settings]") {
    GIVEN("A directory and filename referencing a json config file") {
        std::string settingsFilePrefix{"."};
        settingsFilePrefix += '/';
        std::string settingsFile{"testConfig.json"};

        WHEN("A Settings object is initialized with this directory and filename") {
            Settings settings{settingsFilePrefix, settingsFile};
            THEN("The Settings object should reflect the configuration values in the json file") {
                REQUIRE(settings.resultsDir == "testRes");
                REQUIRE(settings.numberOfBodies == 500);
            }
        }
    }
}
