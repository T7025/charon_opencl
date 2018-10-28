//
// Created by thomas on 3/18/18.
//

#include <gtest/gtest.h>
#include <bruteForceImpl/BruteForceUniverse.h>
#include <spdlog/sinks/null_sink.h>
#include <universeShapes/BinaryUniverseShape.h>
#include <universeShapes/RandomCubeUniverseShape.h>
#include <bruteForceImpl/BruteForceMultiThreadUniverse.h>
//#include <impl1/BruteForceOffloadUniverse.h>
#include <bruteForceImpl/BruteForceOffloadUniverse3.h>

TEST(BruteForceUniverse, BinaryUniverseShape
) {
auto settings = std::make_shared<Settings>();
settings->fileSettings->
enableFileOutput = false;
settings->universeSettings->
timeStep = 0.001;
settings->universeSettings->
gravitationalConstant = 1.0;
settings->universeSettings->
useCollisions = false;

auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());
auto universe = std::make_shared<BruteForceUniverse>(settings, console);
universe->

initBodies (std::make_shared<BinaryUniverseShape>());

universe->

calculateFirstStep();

fp maxDeviation = 0;
for (
int i = 0;
i < 10000; ++i) {
universe->

calculateNextStep();

auto positions = universe->getPositions();
auto dist1 = positions[0].norm();
auto dist2 = positions[1].norm();
maxDeviation = std::max(maxDeviation, std::abs(dist1 - 1.0));
maxDeviation = std::max(maxDeviation, std::abs(dist2 - 1.0));
//EXPECT_NEAR(dist1, 1.0, 0.00001);
//EXPECT_NEAR(dist2, 1.0, 0.00001);
}
EXPECT_NEAR(maxDeviation,
0.0, 0.00001);
std::cout << "Maximum error: " << maxDeviation <<
std::endl;
}

TEST(BruteForceUniverse, BruteForceMultiThreadUniverse
) {
auto settings = std::make_shared<Settings>();
settings->fileSettings->
enableFileOutput = false;
settings->universeSettings->
timeStep = 0.001;
settings->universeSettings->
gravitationalConstant = 1.0;
settings->universeSettings->
useCollisions = false;
settings->universeSettings->
number_bodies = 256;
auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());

auto singleThreadUniverse = std::make_shared<BruteForceUniverse>(settings, console);
auto multiThreadUniverse = std::make_shared<BruteForceMultiThreadUniverse>(settings, console);

singleThreadUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

multiThreadUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

fp maxDeviation = 0;


for (
int i = 0;
i < 100; ++i) {
singleThreadUniverse->

calculateNextStep();

multiThreadUniverse->

calculateNextStep();

for (
int bodyIndex = 0;
bodyIndex<settings->universeSettings->
number_bodies;
++bodyIndex) {
auto pos1 = singleThreadUniverse->getPositions()[bodyIndex];
auto pos2 = multiThreadUniverse->getPositions()[bodyIndex];
maxDeviation = std::max(maxDeviation, pos1.distance(pos2));
}
}
EXPECT_NEAR(maxDeviation,
0.0, 1E-5);
std::cout << "Maximum error: " << maxDeviation <<
std::endl;
}

/*TEST(BruteForceUniverse, BruteForceOffloadUniverse) {
    auto settings = std::make_shared<Settings>();
    settings->fileSettings->enableFileOutput = false;
    settings->universeSettings->timeStep = 0.001;
    settings->universeSettings->gravitationalConstant = 1.0;
    settings->universeSettings->useCollisions = false;
    settings->universeSettings->number_bodies = 256;
    auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());

    auto singleThreadUniverse = std::make_shared<BruteForceUniverse>(settings, console);
    auto offloadUniverse = std::make_shared<BruteForceOffloadUniverse>(settings, console);

    singleThreadUniverse->initBodies(std::make_shared<RandomCubeUniverseShape>());
    offloadUniverse->initBodies(std::make_shared<RandomCubeUniverseShape>());
    fp maxDeviation = 0;
    for (int i = 0; i < 100; ++i) {
        singleThreadUniverse->calculateNextStep();
        offloadUniverse->calculateNextStep();

        for (int bodyIndex = 0; bodyIndex < settings->universeSettings->number_bodies; ++bodyIndex) {
            auto pos1 = singleThreadUniverse->getPositions()[bodyIndex];
            auto pos2 = offloadUniverse->getPositions()[bodyIndex];
            maxDeviation = std::max(maxDeviation, pos1.distance(pos2));
        }
    }
    EXPECT_NEAR(maxDeviation, 0.0, 1E-5);
    std::cout << "Maximum error: " << maxDeviation << std::endl;
}*/

TEST(BruteForceUniverse, BruteForceOffloadUniverse3
) {
auto settings = std::make_shared<Settings>();
settings->fileSettings->
enableFileOutput = false;
settings->universeSettings->
timeStep = 0.001;
settings->universeSettings->
gravitationalConstant = 1.0;
settings->universeSettings->
useCollisions = false;
settings->universeSettings->
number_bodies = 256;
auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());

auto singleThreadUniverse = std::make_shared<BruteForceUniverse>(settings, console);
auto offloadUniverse = std::make_shared<BruteForceOffloadUniverse3>(settings, console);

singleThreadUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

offloadUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

fp maxDeviation = 0;
for (
int i = 0;
i < 100; ++i) {
singleThreadUniverse->

calculateNextStep();

offloadUniverse->

calculateNextStep();

const auto positions1 = singleThreadUniverse->getPositions();
const auto positions2 = offloadUniverse->getPositions();
for (
int bodyIndex = 0;
bodyIndex<settings->universeSettings->
number_bodies;
++bodyIndex) {
auto pos1 = positions1[bodyIndex];
auto pos2 = positions2[bodyIndex];
maxDeviation = std::max(maxDeviation, pos1.distance(pos2));
}
}
EXPECT_NEAR(maxDeviation,
0.0, 1E-5);
std::cout << "Maximum error: " << maxDeviation <<
std::endl;
}
