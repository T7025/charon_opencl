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

template <typename FP>
class Universe<Algorithm::bruteForce, Platform::openCLVec, FP> : public UniverseBase, public OpenCLBase {
public:
    typedef typename CLFloatTypeGet<FP>::cl_fp_vec cl_fp_vec;

    explicit Universe(Settings settings);// : UniverseBase{std::move(settings)} {};

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override;


    void logInternalState(std::ostream &out) override {
        cl::copy(*queue, *xPositionBuffer, xPosition.begin(), xPosition.end());
        cl::copy(*queue, *yPositionBuffer, yPosition.begin(), yPosition.end());
        cl::copy(*queue, *zPositionBuffer, zPosition.begin(), zPosition.end());
        cl::copy(*queue, *xVelocityBuffer, xVelocity.begin(), xVelocity.end());
        cl::copy(*queue, *yVelocityBuffer, yVelocity.begin(), yVelocity.end());
        cl::copy(*queue, *zVelocityBuffer, zVelocity.begin(), zVelocity.end());
        cl::copy(*queue, *xAccelerationBuffer, xAcceleration.begin(), xAcceleration.end());
        cl::copy(*queue, *yAccelerationBuffer, yAcceleration.begin(), yAcceleration.end());
        cl::copy(*queue, *zAccelerationBuffer, zAcceleration.begin(), zAcceleration.end());
        queue->flush();

        out << "mass xPos yPos zPos xVel yVel zVel xAcc yAcc zAcc\n";
        for (unsigned i = 0; i < mass.size(); ++i) {
            for (unsigned j = 0; j < vecSize; ++j) {
                if (mass[i].s[j] != 0) {
                    out << mass[i].s[j] << ' '
                        << xPosition[i].s[j] << ' '
                        << yPosition[i].s[j] << ' '
                        << zPosition[i].s[j] << ' '
                        << xVelocity[i].s[j] << ' '
                        << yVelocity[i].s[j] << ' '
                        << zVelocity[i].s[j] << ' '
                        << xAcceleration[i].s[j] << ' '
                        << yAcceleration[i].s[j] << ' '
                        << zAcceleration[i].s[j] << '\n';
                }
            }
        }
    }

    void step(unsigned int numSteps) override {
        for (unsigned step = 0; step < numSteps; ++step) {
            calcNextStep();
            doneFirstAccCalc = doneFirstStep;
            doneFirstStep = true;
        }
    }

    void finish() override {
        queue->finish();
    };

private:
    cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, FP, FP, unsigned, unsigned, unsigned, unsigned, unsigned> calcNextStepKernel;

    inline void calcNextStep() {
        calcNextStepKernel(*eArgs,
                           *massBuffer,
                           *xPositionBuffer,
                           *yPositionBuffer,
                           *zPositionBuffer,
                           *xVelocityBuffer,
                           *yVelocityBuffer,
                           *zVelocityBuffer,
                           *xAccelerationBuffer,
                           *yAccelerationBuffer,
                           *zAccelerationBuffer,
                           settings.timeStep,
                           settings.softeningLength,
                           settings.numberOfBodies,
                           (unsigned) mass.size(),
                           vecSize,
                           (unsigned) doneFirstStep,
                           (unsigned) doneFirstAccCalc
        );
    }

    std::vector<cl_fp_vec> mass;
    std::vector<cl_fp_vec> xPosition;
    std::vector<cl_fp_vec> yPosition;
    std::vector<cl_fp_vec> zPosition;
    std::vector<cl_fp_vec> xVelocity;
    std::vector<cl_fp_vec> yVelocity;
    std::vector<cl_fp_vec> zVelocity;
    std::vector<cl_fp_vec> xAcceleration;
    std::vector<cl_fp_vec> yAcceleration;
    std::vector<cl_fp_vec> zAcceleration;
    static constexpr unsigned vecSize = sizeof(cl_fp_vec) / sizeof(FP);

    unsigned localWorkSize;
    unsigned globalWorkSize;
    std::unique_ptr<cl::EnqueueArgs> eArgs;

    bool doneFirstAccCalc = false;

    std::unique_ptr<cl::Buffer> massBuffer;
    std::unique_ptr<cl::Buffer> xPositionBuffer;
    std::unique_ptr<cl::Buffer> yPositionBuffer;
    std::unique_ptr<cl::Buffer> zPositionBuffer;
    std::unique_ptr<cl::Buffer> xVelocityBuffer;
    std::unique_ptr<cl::Buffer> yVelocityBuffer;
    std::unique_ptr<cl::Buffer> zVelocityBuffer;
    std::unique_ptr<cl::Buffer> xAccelerationBuffer;
    std::unique_ptr<cl::Buffer> yAccelerationBuffer;
    std::unique_ptr<cl::Buffer> zAccelerationBuffer;
};

template <typename FP>
Universe<Algorithm::bruteForce, Platform::openCLVec, FP>::Universe(Settings settings) :
        UniverseBase{std::move(settings)},
        OpenCLBase{[]() {
            std::filesystem::path kernelFileName{__FILE__};
            kernelFileName.replace_filename("UniverseKernel.cl");
            std::ostringstream ss;
            ss << "#define FP " << CLFloatTypeGet<FP>::fpName << "\n";
            ss << "#define FP3 " << CLFloatTypeGet<FP>::fpName << "3" << "\n";
            ss << "#define fp_vec " << CLFloatTypeGet<FP>::fpVecName << "\n";
            const cl::Program::Sources sources{ss.str() + OpenCLBase::getKernelSource(kernelFileName)};
            return sources;
        }()},
        calcNextStepKernel{program, std::string{"calcNextStep"}} {}


template <typename FP>
void Universe<Algorithm::bruteForce, Platform::openCLVec, FP>::init(std::unique_ptr<BodyGenerator> bodyGenerator) {
    for (unsigned i = 0; i < settings.numberOfBodies;) {
        cl_fp_vec tempMass{0};
        cl_fp_vec tempXPosition;
        cl_fp_vec tempYPosition;
        cl_fp_vec tempZPosition;
        cl_fp_vec tempXVelocity;
        cl_fp_vec tempYVelocity;
        cl_fp_vec tempZVelocity;
        for (unsigned j = 0; j < vecSize && i < settings.numberOfBodies; ++j, ++i) {
            auto[m, pos, vel] = bodyGenerator->getBody();
            tempMass.s[j] = m;
            tempXPosition.s[j] = pos.x;
            tempYPosition.s[j] = pos.y;
            tempZPosition.s[j] = pos.z;
            tempXVelocity.s[j] = vel.x;
            tempYVelocity.s[j] = vel.y;
            tempZVelocity.s[j] = vel.z;
        }
        mass.emplace_back(tempMass);
        xPosition.emplace_back(tempXPosition);
        yPosition.emplace_back(tempYPosition);
        zPosition.emplace_back(tempZPosition);
        xVelocity.emplace_back(tempXVelocity);
        yVelocity.emplace_back(tempYVelocity);
        zVelocity.emplace_back(tempZVelocity);
        xAcceleration.emplace_back(cl_fp_vec{0});
        yAcceleration.emplace_back(cl_fp_vec{0});
        zAcceleration.emplace_back(cl_fp_vec{0});
    }

    massBuffer = bufferFromVector(std::move(mass), CL_MEM_READ_ONLY | CL_MEM_HOST_READ_ONLY);
    xPositionBuffer = bufferFromVector(std::move(xPosition), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    yPositionBuffer = bufferFromVector(std::move(yPosition), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    zPositionBuffer = bufferFromVector(std::move(zPosition), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    xVelocityBuffer = bufferFromVector(std::move(xVelocity), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    yVelocityBuffer = bufferFromVector(std::move(yVelocity), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    zVelocityBuffer = bufferFromVector(std::move(zVelocity), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    xAccelerationBuffer = bufferFromVector(std::move(xAcceleration), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    yAccelerationBuffer = bufferFromVector(std::move(yAcceleration), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    zAccelerationBuffer = bufferFromVector(std::move(zAcceleration), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);

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
