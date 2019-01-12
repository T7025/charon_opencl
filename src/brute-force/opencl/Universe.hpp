//
// Created by thomas on 11/14/18.
//

#pragma once

#include <base/Util.hpp>
#include <base/Universe.hpp>
#include <base/Vec3.hpp>
#include <base/OpenCLBase.hpp>
#include <vector>
#include <iostream>
#include <cassert>
#include <CL/cl2.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>


template <typename FP> struct CLFloatTypeGet {};
template <> struct CLFloatTypeGet<double> {
    typedef double value_type;
    typedef cl_double3 value_type3;
    static constexpr const char *fpName = "double";
};
template <> struct CLFloatTypeGet<float> {
    typedef float value_type;
    typedef cl_float3 value_type3;
    static constexpr const char *fpName = "float";
};

std::ostream &operator<<(std::ostream &out, const cl_double3 &val) {
    out << val.x << ' ' << val.y << ' ' << val.z;
    return out;
}

std::ostream &operator<<(std::ostream &out, const cl_float3 &val) {
    out << val.x << ' ' << val.y << ' ' << val.z;
    return out;
}

template <typename FP>
class Universe<Algorithm::bruteForce, Platform::openCL, FP> : public UniverseBase, public OpenCLBase {
public:
    typedef typename CLFloatTypeGet<FP>::value_type3 cl_fp3;
    typedef typename CLFloatTypeGet<FP>::value_type cl_fp;

    explicit Universe(Settings settings);// : UniverseBase{std::move(settings)} {};

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override;


    void logInternalState(std::ostream &out) override {
        assert(mass.size() == position.size() && mass.size() == velocity.size() && mass.size() == acceleration.size());

        cl::copy(*queue, *positionBuffer, position.begin(), position.end());
        cl::copy(*queue, *velocityBuffer, velocity.begin(), velocity.end());
        cl::copy(*queue, *accelerationBuffer, acceleration.begin(), acceleration.end());
        queue->flush();

        out << "mass xPos yPos zPos xVel yVel zVel xAcc yAcc zAcc\n";
        for (unsigned i = 0; i < mass.size(); ++i) {
            out << mass[i] << ' ' << position[i] << ' ' << velocity[i] << ' ' << acceleration[i] << '\n';
        }
    }

    void step(unsigned int numSteps) override {
        if (!doneFirstStep) {
            calcNextPosition();
            calcAcceleration(calcFirstAccelerationKernel);
            doneFirstStep = true;
            numSteps--;
        }

        for (unsigned step = 0; step < numSteps; ++step) {
            calcNextPosition();
            calcAcceleration(calcAccelerationKernel);
        }
    }

    void finish() override {
        queue->finish();
    };

private:
    cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl_fp, int> calcNextPositionKernel;
    typedef cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::LocalSpaceArg, cl::LocalSpaceArg, cl_fp, cl_fp, int> CalcAccKernelFunctor;
    CalcAccKernelFunctor calcFirstAccelerationKernel;
    CalcAccKernelFunctor calcAccelerationKernel;

    inline void calcNextPosition() {
        calcNextPositionKernel(*eArgs,
                               *positionBuffer, *velocityBuffer, *accelerationBuffer, settings.timeStep,
                               (int) mass.size());
    }

    inline void calcAcceleration(CalcAccKernelFunctor &calcAccKernel) {
        calcAccKernel(*eArgs,
                      *massBuffer, *positionBuffer, *velocityBuffer, *accelerationBuffer,
                      cl::Local(localWorkSize * sizeof(FP)), cl::Local(localWorkSize * sizeof(cl_fp3)),
                      settings.timeStep, settings.softeningLength, (int) mass.size());
    }

    std::vector<FP> mass;
    std::vector<cl_fp3> position;
    std::vector<cl_fp3> velocity;
    std::vector<cl_fp3> acceleration;

    unsigned localWorkSize;
    unsigned globalWorkSize;
    std::unique_ptr<cl::EnqueueArgs> eArgs;

    std::unique_ptr<cl::Buffer> massBuffer;
    std::unique_ptr<cl::Buffer> positionBuffer;
    std::unique_ptr<cl::Buffer> velocityBuffer;
    std::unique_ptr<cl::Buffer> accelerationBuffer;
};

template <typename FP>
Universe<Algorithm::bruteForce, Platform::openCL, FP>::Universe(Settings settings) :
        UniverseBase{std::move(settings)},
        OpenCLBase{[]() {
            std::filesystem::path kernelFileName{__FILE__};
            kernelFileName.replace_filename("UniverseKernel.cl");
            std::ostringstream ss;
            ss << "#define FP " << CLFloatTypeGet<FP>::fpName << "\n";
            ss << "#define FP3 " << CLFloatTypeGet<FP>::fpName << "3" << "\n";
            const cl::Program::Sources sources{ss.str() + OpenCLBase::getKernelSource(kernelFileName)};
            return sources;
        }()},
        calcNextPositionKernel{program, std::string{"calcNextPosition"}},
        calcFirstAccelerationKernel{program, std::string{"calcFirstVecAndAcc"}},
        calcAccelerationKernel{program, std::string{"calcVecAndAcc"}} {
}


template <typename FP>
void Universe<Algorithm::bruteForce, Platform::openCL, FP>::init(std::unique_ptr<BodyGenerator> bodyGenerator) {
    for (unsigned i = 0; i < settings.numberOfBodies; ++i) {
        auto[m, pos, vel] = bodyGenerator->getBody();
        mass.emplace_back(cl_fp(m));
        position.emplace_back(cl_fp3{cl_fp(pos.x), cl_fp(pos.y), cl_fp(pos.z)});
        velocity.emplace_back(cl_fp3{cl_fp(vel.x), cl_fp(vel.y), cl_fp(vel.z)});
        acceleration.emplace_back(cl_fp3{0, 0, 0});
    }


    massBuffer = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                              mass.size() * sizeof(cl_fp), mass.data());
    positionBuffer = std::make_unique<cl::Buffer>(*context,
                                                  CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                                  position.size() * sizeof(cl_fp3), position.data());
    velocityBuffer = std::make_unique<cl::Buffer>(*context,
                                                  CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                                  velocity.size() * sizeof(cl_fp3), velocity.data());
    accelerationBuffer = std::make_unique<cl::Buffer>(*context,
                                                      CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                                      acceleration.size() * sizeof(cl_fp3), acceleration.data());

    localWorkSize = 64;
    globalWorkSize = (unsigned) (mass.size() / localWorkSize + 1) * localWorkSize;
    eArgs = std::make_unique<cl::EnqueueArgs>(
            *queue,
            cl::NullRange,
            cl::NDRange(globalWorkSize),
            cl::NDRange(localWorkSize)
    );
//    std::cout << " - localWorkSize: " << localWorkSize << ", globalWorkSize: " << globalWorkSize << "\n";

//    std::cout << "Size of mass values: " << massBuffer->getInfo<CL_MEM_SIZE>() << " bytes\n";
}
