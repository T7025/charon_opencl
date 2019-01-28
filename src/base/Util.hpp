//
// Created by thomas on 03/11/18.
//

#pragma once

#ifdef CHARON_USE_DOUBLE
using fp = double;
#else
using fp = float;
#endif

enum class Algorithm {
    bruteForce,
    barnesHut
};

enum class Platform {
    cpuSingleThread,
    cpuMultiThread,
    openCL,
    openCLLoc,
    openCLVec,
};
