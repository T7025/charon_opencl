//
// Created by thomas on 02/11/18.
//

#ifndef CHARON_OPENCL_SETTINGS_HPP
#define CHARON_OPENCL_SETTINGS_HPP


#include <string>
#include <base/Util.hpp>

class Settings {
public:
    bool enableFileOutput;
    std::string resultsDirectory;
    std::string resultsFilenamePrefix;

    unsigned nrOfSteps;  ///< Total nr. of steps to simulate.
    unsigned snapshotDelta;  ///< Amount of steps between each snapshot of the universe.
    std::string algorithm;
    std::string platform;
    std::string implementationType;  ///< What kind of implementation will be used (Brute force vs. Barnes-Hut, CPU vs. GPU)
    std::string universeShape;  ///< Initial shape of the universe.

    fp barnesHutCutoff;  ///< When using a Barnes-Hut implementation.

    long rngSeed;
    fp timeStep;

    unsigned numberOfBodies;  ///< Number of bodies to simulate.

    fp universeRadius;  ///< Radius of the universe in km


};


#endif //CHARON_OPENCL_SETTINGS_HPP
