//
// Created by thomas on 12/3/18.
//

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>
#include <settings/Settings.hpp>
#include <base/BodyGenerator.hpp>
#include <base/Universe.hpp>
#include <base/UniverseBuilder.hpp>
#include <base/Exceptions.hpp>

class SingleBodyGenerator : public BodyGenerator {
public:
    using BodyGenerator::BodyGenerator;

    std::tuple<fp, Vec3<fp>, Vec3<fp>> getBody() override {
        return {1, {0, 0, 0}, {1, 1, 1}};
    }
};

SCENARIO("Test the position update function", "[universe]") {
    GIVEN("For every possible universe configuration") {
        std::vector<nlohmann::json> settings{nlohmann::json{}};

        auto addSettings = [&settings](const std::string &name, auto ...values) {
            auto addSettingsImpl = [&settings](auto &self, size_t size, const std::string &name, auto &&value,
                                               auto &&...values) {
                if constexpr(sizeof...(values) > 0) {
                    self(self, size, name, values...);
                }
                for (unsigned i = 0; i < size; ++i) {
                    auto tempSettings = settings[i];
                    tempSettings[name] = value;
                    settings[i + size * sizeof...(values)] = tempSettings;
                }
            };
            const auto size = settings.size();
            settings.resize(settings.size() * sizeof...(values));
            addSettingsImpl(addSettingsImpl, size, name, values...);
        };

        addSettings("algorithm", "brute-force", "barnes-hut");
        addSettings("platform", "cpu-single-thread", "cpu-multi-thread", "opencl");
//        addSettings("numberOfBodies", 1);
//        addSettings("bodyGeneratorType", )

//        Universe()
        for (const auto &s : settings) {
            std::unique_ptr<UniverseBase> universe;
            try {
                universe = UniverseBuilder{}(Settings{s});
            }
            catch (NotImplementedUniverseException &e) {}

            universe->init(std::make_unique<SingleBodyGenerator>(settings));



        }



    }
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