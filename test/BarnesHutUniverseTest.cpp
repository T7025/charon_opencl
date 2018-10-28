//
// Created by thomas on 4/27/18.
//

#include <gtest/gtest.h>
#include <barnesHutTreeImpl/BarnesHutTreeUniverse.h>
#include <spdlog/sinks/null_sink.h>
#include <universeShapes/BinaryUniverseShape.h>
#include <bruteForceImpl/BruteForceUniverse.h>
#include <universeShapes/RandomCubeUniverseShape.h>
#include <barnesHutSFCImpl/BarnesHutUniverse.h>
#include <barnesHutSFCImpl/BarnesHutMultiThreadUniverse.h>
#include <barnesHutSFCImpl/BarnesHutOffloadUniverse.h>

TEST(BarnesHutTreeUniverse, BinaryUniverseShape
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
auto universe = std::make_shared<BarnesHutTreeUniverse>(settings, console);
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

TEST(BarnesHutTreeUniverse, CompareToBruteForce
) {
auto settings = std::make_shared<Settings>();
settings->fileSettings->
enableFileOutput = false;
settings->universeSettings->
timeStep = 0.0001;
settings->universeSettings->
gravitationalConstant = 1.0;
settings->universeSettings->
useCollisions = false;
settings->universeSettings->
number_bodies = 256;
auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());

auto bruteForceUniverse = std::make_shared<BruteForceUniverse>(settings, console);
auto barnesHutUniverse = std::make_shared<BarnesHutTreeUniverse>(settings, console);

bruteForceUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

barnesHutUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

fp maxDeviation = 0;
for (
int i = 0;
i < 10; ++i) {
bruteForceUniverse->

calculateNextStep();

barnesHutUniverse->

calculateNextStep();

for (
int bodyIndex = 0;
bodyIndex<settings->universeSettings->
number_bodies;
++bodyIndex) {
auto pos1 = bruteForceUniverse->getPositions()[bodyIndex];
auto pos2 = barnesHutUniverse->getPositions()[bodyIndex];
maxDeviation = std::max(maxDeviation, pos1.distance(pos2));
}
}
EXPECT_NEAR(maxDeviation,
0.0, 1E-5);
std::cout << "Maximum error: " << maxDeviation <<
std::endl;
}

TEST(BarnesHutUniverse, BinaryUniverseShape
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
settings->simulatorSettings->
barnesHutCutoff = 0.7;
auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());
auto universe = std::make_shared<BarnesHutUniverse>(settings, console);
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

TEST(BarnesHutUniverse, CompareToBruteForce
) {
auto settings = std::make_shared<Settings>();
settings->fileSettings->
enableFileOutput = false;
settings->universeSettings->
timeStep = 0.0001;
settings->universeSettings->
gravitationalConstant = 1.0;
settings->universeSettings->
useCollisions = false;
settings->universeSettings->
number_bodies = 256;
settings->simulatorSettings->
barnesHutCutoff = 0.7;
auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());

auto bruteForceUniverse = std::make_shared<BruteForceUniverse>(settings, console);
auto barnesHutUniverse = std::make_shared<BarnesHutUniverse>(settings, console);

bruteForceUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

barnesHutUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

bruteForceUniverse->

calculateFirstStep();

barnesHutUniverse->

calculateFirstStep();

fp maxDeviation = 0;
for (
int i = 0;
i < 10; ++i) {
bruteForceUniverse->

calculateNextStep();

barnesHutUniverse->

calculateNextStep();

auto positions1 = bruteForceUniverse->getPositions();
auto positions2 = barnesHutUniverse->getPositions();
auto comp = [](const Vector3 &lhs, const Vector3 &rhs) {
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y || (lhs.y == rhs.y && lhs.z < rhs.z));
};
std::sort(positions1
.

begin(), positions1

.

end(), comp

);
std::sort(positions2
.

begin(), positions2

.

end(), comp

);

for (
int bodyIndex = 0;
bodyIndex<settings->universeSettings->
number_bodies;
++bodyIndex) {
maxDeviation = std::max(maxDeviation, positions1[bodyIndex].distance(positions2[bodyIndex]));
}
}
EXPECT_NEAR(maxDeviation,
0.0, 1E-5);
std::cout << "Maximum error: " << maxDeviation <<
std::endl;
}

TEST(BarnesHutUniverse, CompareToBarnesHutTree
) {
auto settings = std::make_shared<Settings>();
settings->fileSettings->
enableFileOutput = false;
settings->universeSettings->
timeStep = 0.0001;
settings->universeSettings->
gravitationalConstant = 1.0;
settings->universeSettings->
useCollisions = false;
settings->universeSettings->
number_bodies = 256;
settings->simulatorSettings->
barnesHutCutoff = 0.5;
auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());

auto barnesHutTreeUniverse = std::make_shared<BarnesHutTreeUniverse>(settings, console);
auto barnesHutUniverse = std::make_shared<BarnesHutUniverse>(settings, console);

barnesHutTreeUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

barnesHutUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

barnesHutTreeUniverse->

calculateFirstStep();

barnesHutUniverse->

calculateFirstStep();

fp maxDeviation = 0;
for (
int i = 0;
i < 10; ++i) {
barnesHutTreeUniverse->

calculateNextStep();

barnesHutUniverse->

calculateNextStep();

auto positions1 = barnesHutTreeUniverse->getPositions();
auto positions2 = barnesHutUniverse->getPositions();
auto comp = [](const Vector3 &lhs, const Vector3 &rhs) {
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y || (lhs.y == rhs.y && lhs.z < rhs.z));
};
std::sort(positions1
.

begin(), positions1

.

end(), comp

);
std::sort(positions2
.

begin(), positions2

.

end(), comp

);

for (
int bodyIndex = 0;
bodyIndex<settings->universeSettings->
number_bodies;
++bodyIndex) {
maxDeviation = std::max(maxDeviation, positions1[bodyIndex].distance(positions2[bodyIndex]));
}
}
EXPECT_NEAR(maxDeviation,
0.0, 1E-5);
std::cout << "Maximum error: " << maxDeviation <<
std::endl;
}


TEST(BarnesHutUniverse, BarnesHutMultiThreadUniverse
) {
auto settings = std::make_shared<Settings>();
settings->fileSettings->
enableFileOutput = false;
settings->universeSettings->
timeStep = 0.0001;
settings->universeSettings->
gravitationalConstant = 1.0;
settings->universeSettings->
useCollisions = false;
settings->universeSettings->
number_bodies = 256;
auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());

auto barnesHutUniverse = std::make_shared<BarnesHutUniverse>(settings, console);
auto barnesHutMultiThreadUniverse = std::make_shared<BarnesHutMultiThreadUniverse>(settings, console);

barnesHutUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

barnesHutMultiThreadUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

barnesHutUniverse->

calculateFirstStep();

barnesHutMultiThreadUniverse->

calculateFirstStep();

fp maxDeviation = 0;
for (
int i = 0;
i < 100; ++i) {
barnesHutUniverse->

calculateNextStep();

barnesHutMultiThreadUniverse->

calculateNextStep();

auto positions1 = barnesHutUniverse->getPositions();
auto positions2 = barnesHutMultiThreadUniverse->getPositions();
auto comp = [](const Vector3 &lhs, const Vector3 &rhs) {
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y || (lhs.y == rhs.y && lhs.z < rhs.z));
};
std::sort(positions1
.

begin(), positions1

.

end(), comp

);
std::sort(positions2
.

begin(), positions2

.

end(), comp

);
for (
int bodyIndex = 0;
bodyIndex<settings->universeSettings->
number_bodies;
++bodyIndex) {
maxDeviation = std::max(maxDeviation, positions1[bodyIndex].distance(positions2[bodyIndex]));
}
}
EXPECT_NEAR(maxDeviation,
0.0, 1E-5);
std::cout << "Maximum error: " << maxDeviation <<
std::endl;
}

TEST(BarnesHutUniverse, BarnesHutOffloadUniverse
) {
auto settings = std::make_shared<Settings>();
settings->fileSettings->
enableFileOutput = false;
settings->universeSettings->
timeStep = 0.0001;
settings->universeSettings->
gravitationalConstant = 1.0;
settings->universeSettings->
useCollisions = false;
settings->universeSettings->
number_bodies = 256;
auto console = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::null_sink_mt>());

auto barnesHutUniverse = std::make_shared<BarnesHutUniverse>(settings, console);
auto barnesHutOffloadUniverse = std::make_shared<BarnesHutOffloadUniverse>(settings, console);

barnesHutUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

barnesHutOffloadUniverse->

initBodies (std::make_shared<RandomCubeUniverseShape>());

barnesHutUniverse->

calculateFirstStep();

barnesHutOffloadUniverse->

calculateFirstStep();

fp maxDeviation = 0;
for (
int i = 0;
i < 100; ++i) {
barnesHutUniverse->

calculateNextStep();

barnesHutOffloadUniverse->

calculateNextStep();

auto positions1 = barnesHutUniverse->getPositions();
auto positions2 = barnesHutOffloadUniverse->getPositions();
auto comp = [](const Vector3 &lhs, const Vector3 &rhs) {
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y || (lhs.y == rhs.y && lhs.z < rhs.z));
};
std::sort(positions1
.

begin(), positions1

.

end(), comp

);
std::sort(positions2
.

begin(), positions2

.

end(), comp

);
for (
int bodyIndex = 0;
bodyIndex<settings->universeSettings->
number_bodies;
++bodyIndex) {
maxDeviation = std::max(maxDeviation, positions1[bodyIndex].distance(positions2[bodyIndex]));
}
}
EXPECT_NEAR(maxDeviation,
0.0, 1E-5);
std::cout << "Maximum error: " << maxDeviation <<
std::endl;
}
