//
// Created by thomas on 02/11/18.
//

#pragma once


#include <string>
#include <base/Util.hpp>
#include <nlohmann/json.hpp>


class Settings {
public:
    explicit Settings(const nlohmann::json &json);

    Settings(const std::string &settingsFilePrefix, const std::string &settingsFile);

    // IO Settings
    const bool enableFileOutput;
    const std::string resultsDir;
    const std::string resultsFilenamePrefix;


    const unsigned nrOfSteps;  ///< Total nr. of steps to simulate.
    const unsigned snapshotDelta;  ///< Amount of steps between each snapshot of the universe.
    const std::string algorithm;
    const std::string platform;
    const std::string floatingPointType; ///< float or double.
    const std::string bodyGeneratorType;  ///< Defines initial shape of the universe.

    const fp barnesHutCutoff;  ///< Used when using a Barnes-Hut implementation.

    const unsigned long rngSeed;
    const fp timeStep;

    const unsigned numberOfBodies;  ///< Number of bodies to simulate.

    const fp universeRadius;  ///< Radius of the universe.
    const fp totalMass;  ///< Total mass of the system.
    const fp G;  ///< Gravitational constant.
    /// Softening length to avoid large angle scatterings and bound particle pairs (when simulating multiple bodies as a single large body)
    /// (From High performance computing and numerical modelling (Heidelberg University) (arXiv:1412.5187v1 [astro-ph.GA] 16 Dec 2014))
    /// v^2 >> G*m/e
    const fp softeningLength;

    const int numThreads;  ///< Number of OpenMP threads allowed.
    const std::string openclCompileOpts;  ///< OpenCL compile options.
};
