//
// Created by thomas on 01/11/18.
//

#ifndef CHARON_OPENCL_SIMULATOR_HPP
#define CHARON_OPENCL_SIMULATOR_HPP


#include <memory>

class Simulator {
public:
    Simulator(std::shared_ptr<Settings> settings);

    void setup();

    void run();

private:
    const unsigned nrOfSteps;
    const unsigned snapshotDelta;


};


#endif //CHARON_OPENCL_SIMULATOR_HPP
