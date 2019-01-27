//
// Created by thomas on 27/01/19.
//

#ifndef CHARON_OPENCL_UNIVERSE_HPP
#define CHARON_OPENCL_UNIVERSE_HPP

#include <base/Util.hpp>
#include <base/Universe.hpp>
#include <base/OpenCLBase.hpp>

template <typename FP>
class Universe<Algorithm::barnesHut, Platform::openCL, FP> : public UniverseBase, public OpenCLBase {
public:
    explicit Universe(Settings settings);

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override;

    void logInternalState(std::ostream &out) override;

    void finish() override;

private:
    void calcNextStep() override;

    void calculateFirstAcc();

    void calculateNextAcc();

    void calcNextPosition();

};


#endif //CHARON_OPENCL_UNIVERSE_HPP
