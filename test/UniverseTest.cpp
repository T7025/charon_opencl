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

//        addSettings("algorithm", "brute-force", "barnes-hut");
        addSettings("algorithm", "barnes-hut");
        addSettings("platform", "cpu-single-thread", "cpu-multi-thread", "opencl", "openclloc", "openclvec");
//        addSettings("platform", "openclvec");
        addSettings("floatingPointType", "float", "double");
//        addSettings("floatingPointType", "float");
        addSettings("barnesHutCutoff", 0.0);
        addSettings("rngSeed", 1302);
//        if (false)
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
                    CHECK(vel == Vec3<fp>{1, 1, 1});
                    CHECK(acc == Vec3<fp>{0, 0, 0});

                    universe->step(1);
                }
            }
        }
//        if (false)
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
//        if (false)
        WHEN("Computing the evolution of a 128 body system") {
            int numBodies = 128;
            double timeStep = 0.001;
//            unsigned numSteps = 2;
            unsigned numSteps = 2;

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

            auto sortResults = [](const auto &lhs, const auto &rhs) {
                const auto &[mass1, pos1, vel1, acc1] = lhs;
                const auto &[mass2, pos2, vel2, acc2] = rhs;
                return pos1.x < pos2.x ||
                       ((pos1.x == pos2.x && pos1.y < pos2.y) || (pos1.y == pos2.y && pos1.z < pos2.z));
            };

            std::sort(baselineResult.begin(), baselineResult.end(), sortResults);

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
                std::sort(result.begin(), result.end(), sortResults);

                for (unsigned i = 0; i < result.size(); ++i) {
                    const auto &[baselineMass, baselinePos, baselineVel, baselineAcc] = baselineResult[i];
                    const auto &[mass, pos, vel, acc] = result[i];
//                    std::cout << baselineMass << ",\t" << baselinePos <<",\t" << baselineVel << ",\t" << baselineAcc << "\n";
//                    std::cout << mass << ",\t" << pos <<",\t" << vel << ",\t" << acc << "\n\n";
                    REQUIRE(mass == Approx(baselineMass));
//                    REQUIRE(pos.norm() == Approx(baselinePos.norm()));
                    CHECK(pos.norm() == Approx(baselinePos.norm()));
                    CHECK(vel.norm() == Approx(baselineVel.norm()));
                    CHECK(acc.norm() == Approx(baselineAcc.norm()));
                }
            }
        }
    }

}