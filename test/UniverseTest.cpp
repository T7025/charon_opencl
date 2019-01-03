//
// Created by thomas on 12/3/18.
//

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>
#include <settings/Settings.hpp>
#include <base/BodyGenerator.hpp>
#include <base/Universe.hpp>
#include <base/getConcreteUniverse.hpp>
#include <base/Exceptions.hpp>
#include <iostream>
#include <BodyGenerators/BinaryBodyGenerator.hpp>
#include <BodyGenerators/SphereBodyGenerator.hpp>

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
//        addSettings("platform", "cpu-single-thread");
        addSettings("floatingPointType", "float", "double");
        addSettings("rngSeed", 1302);

        WHEN("Computing the evolution of a trivial single body system") {
            addSettings("numberOfBodies", 1);
            addSettings("timeStep", 1);
            for (const auto &s : settings) {
                Settings settings{s};
                std::unique_ptr<UniverseBase> universe;
                try {
                    universe = getConcreteUniverse(Settings{s});
                }
                catch (NotImplementedUniverseException &e) {
                    continue;
                }
                universe->init(std::make_unique<SingleBodyGenerator>(settings));

                for (int i = 0; i < 50; ++i) {
                    auto[mass, pos, vel, acc] = universe->getInternalState()[0];
                    REQUIRE(mass == 1);
                    REQUIRE(pos == Vec3<fp>(i, i, i));
                    REQUIRE(vel == Vec3<fp>{1, 1, 1});
                    REQUIRE(acc == Vec3<fp>{0, 0, 0});

                    universe->step(1);
                }
            }
        }
        WHEN("Computing the evolution of a trivial binary system") {
            addSettings("numberOfBodies", 2);
            addSettings("timeStep", 0.001);
            for (const auto &s : settings) {
                Settings settings{s};
                std::unique_ptr<UniverseBase> universe;
                try {
                    universe = getConcreteUniverse(Settings{s});
                }
                catch (NotImplementedUniverseException &e) {
                    continue;
                }
                universe->init(std::make_unique<BinaryBodyGenerator>(settings));

                for (int i = 0; i < 100; ++i) {
                    for (auto &[mass, pos, vel, acc] : universe->getInternalState()) {
                        //std::cout << mass << ", " << pos << ", " << vel << "\n";
                        REQUIRE(mass == 1);
                        REQUIRE(pos.norm() == Approx(1.0).epsilon(0.00005));
                        REQUIRE(vel.norm() == Approx(0.5).epsilon(0.0001));
                    }
                    //std::cout<<"\n";
                    universe->step(500);
                }
            }
        }
        WHEN("Computing the evolution of a 128 body system") {
            int numBodies = 128;
            double timeStep = 0.001;
            unsigned numSteps = 50;

            addSettings("numberOfBodies", numBodies);
            addSettings("timeStep", timeStep);

            nlohmann::json baselineUniverseSettings{};
            baselineUniverseSettings["numberOfBodies"] = numBodies;
            baselineUniverseSettings["timeStep"] = timeStep;
            baselineUniverseSettings["algorithm"] = "brute-force";
            baselineUniverseSettings["platform"] = "cpu-single-thread";
            baselineUniverseSettings["floatingPointType"] = "double";
            baselineUniverseSettings["rngSeed"] = 1302;

            auto baselineUniverse = getConcreteUniverse(Settings{baselineUniverseSettings});
            baselineUniverse->init(std::make_unique<SphereBodyGenerator>(Settings{baselineUniverseSettings}));
            baselineUniverse->step(numSteps);
            auto baselineResult = baselineUniverse->getInternalState();

            for (const auto &s : settings) {
                std::unique_ptr<UniverseBase> universe;
                try {
                    universe = getConcreteUniverse(Settings{s});
                }
                catch (NotImplementedUniverseException &e) {
                    continue;
                }
                universe->init(std::make_unique<SphereBodyGenerator>(Settings{s}));
                universe->step(numSteps);
                auto result = universe->getInternalState();
                for (unsigned i = 0; i < result.size(); ++i) {
                    const auto &[baselineMass, baselinePos, baselineVel, baselineAcc] = baselineResult[i];
                    const auto &[mass, pos, vel, acc] = result[i];
                    REQUIRE(mass == Approx(baselineMass));
                    REQUIRE(pos.norm() == Approx(baselinePos.norm()));
                    REQUIRE(vel.norm() == Approx(baselineVel.norm()));
                    REQUIRE(acc.norm() == Approx(baselineAcc.norm()));
                }
            }
        }
    }

}