//
// Created by thomas on 11/14/18.
//

#pragma once

#include <base/Util.hpp>
#include <base/Universe.hpp>
#include <base/Vec3.hpp>
#include <vector>
#include <iostream>
#include <cassert>
#include <CL/cl2.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>


template <typename FP> struct CLFloatTypeGet {};
template <> struct CLFloatTypeGet<double> {
//    typedef cl_double value_type;
    typedef double value_type;
    typedef cl_double3 value_type3;
    static constexpr const char *fpName = "double";
};
template <> struct CLFloatTypeGet<float> {
//    typedef cl_float value_type;
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
class Universe<Algorithm::bruteForce, Platform::openCL, FP> : public UniverseBase {
public:
    typedef typename CLFloatTypeGet<FP>::value_type3 cl_fp3;
    typedef typename CLFloatTypeGet<FP>::value_type cl_fp;

    explicit Universe(Settings settings);// : UniverseBase{std::move(settings)} {};

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override;


    void logInternalState(std::ostream &out) override {
        assert(mass.size() == position.size() && mass.size() == velocity.size() && mass.size() == acceleration.size());

//        std::cout << "before: \n";
//        for (const auto &pos : position) {
//            std::cout << pos <<"\n";
//        }

        cl::copy(*queue, *positionBuffer, position.begin(), position.end());
        cl::copy(*queue, *velocityBuffer, velocity.begin(), velocity.end());
        cl::copy(*queue, *accelerationBuffer, acceleration.begin(), acceleration.end());
        queue->flush();

//        std::cout << "after: \n";
//        for (const auto &pos : position) {
//            std::cout << pos <<"\n";
//        }
        out << "mass xPos yPos zPos xVel yVel zVel xAcc yAcc zAcc\n";
        for (unsigned i = 0; i < mass.size(); ++i) {
            out << mass[i] << ' ' << position[i] << ' ' << velocity[i] << ' ' << acceleration[i] << '\n';
//            std::cout << mass[i] << ' ' << position[i] << ' ' << velocity[i] << ' ' << acceleration[i] << '\n';
        }
    }

    void step(unsigned int numSteps) override {
        cl::EnqueueArgs eArgs(
                *queue,
                cl::NullRange,
                cl::NDRange(globalWorkSize),
                cl::NDRange(localWorkSize)
        );
        cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl_fp, int> calcNextPosition(
                program, std::string{"calcNextPosition"}
        );
        // Local: cl::LocalSpaceArg
        cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::LocalSpaceArg, cl::LocalSpaceArg, cl_fp, cl_fp, int> calcFirstAcceleration(
                program, std::string{"calcFirstVecAndAcc"}
        );
        cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::LocalSpaceArg, cl::LocalSpaceArg, cl_fp, cl_fp, int> calcAcceleration(
                program, std::string{"calcVecAndAcc"}
        );

        if (!doneFirstStep) {
            calcNextPosition(eArgs,
                             *positionBuffer, *velocityBuffer, *accelerationBuffer, settings.timeStep,
                             (int) mass.size());
            calcFirstAcceleration(eArgs,
                                  *massBuffer, *positionBuffer, *velocityBuffer, *accelerationBuffer,
                                  cl::Local(localWorkSize * sizeof(FP)), cl::Local(localWorkSize * sizeof(cl_fp3)),
                                  settings.timeStep, settings.softeningLength, (int) mass.size());
            doneFirstStep = true;
            numSteps--;
        }

        for (unsigned step = 0; step < numSteps; ++step) {
            calcNextPosition(eArgs,
                             *positionBuffer, *velocityBuffer, *accelerationBuffer, settings.timeStep,
                             (int) mass.size());
            calcAcceleration(eArgs,
                             *massBuffer, *positionBuffer, *velocityBuffer, *accelerationBuffer,
                             cl::Local(localWorkSize * sizeof(FP)), cl::Local(localWorkSize * sizeof(cl_fp3)),
                             settings.timeStep, settings.softeningLength, (int) mass.size());
        }
    }

    void finish() override {
        queue->finish();
    };

private:
    cl::Platform platform;
    cl::Device device;
    std::unique_ptr<cl::Context> context;
    cl::Program::Sources sources;
    cl::Program program;
    std::unique_ptr<cl::CommandQueue> queue;

    std::vector<FP> mass;
    std::vector<cl_fp3> position;
    std::vector<cl_fp3> velocity;
    std::vector<cl_fp3> acceleration;

    unsigned localWorkSize;
    unsigned globalWorkSize;

    std::unique_ptr<cl::Buffer> massBuffer;
    std::unique_ptr<cl::Buffer> positionBuffer;
    std::unique_ptr<cl::Buffer> velocityBuffer;
    std::unique_ptr<cl::Buffer> accelerationBuffer;
};

template <typename FP>
Universe<Algorithm::bruteForce, Platform::openCL, FP>::Universe(Settings settings) : UniverseBase{std::move(settings)} {

}


template <typename FP>
void Universe<Algorithm::bruteForce, Platform::openCL, FP>::init(std::unique_ptr<BodyGenerator> bodyGenerator) {
    const auto clDeviceType = CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU;
//    const auto clDeviceType = CL_DEVICE_TYPE_GPU;

    // Pick an OpenCL platform that contains the requested device type.
    // Then pick the first available device of the requested type.
    std::vector<cl::Platform> allPlatforms;

    cl::Platform::get(&allPlatforms);

    if (allPlatforms.empty()) {
        throw std::runtime_error{"No openCL platforms found."};
    }

    std::vector<cl::Device> allDevices;
    for (const auto &p : allPlatforms) {
        p.getDevices(clDeviceType, &allDevices);
        if (!allDevices.empty()) {
            platform = p;
            break;
        }
    }

    if (allDevices.empty()) {
        throw std::runtime_error{"No openCL devices found."};
    }
    device = cl::Device(allDevices[0]);

    /*
    std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
    std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";
    auto printVector = [](const auto &vec) {
        std::stringstream ss;
        for (const auto &item : vec) ss << item << " ";
        return ss.str();
    };
    std::cout << "Double supported: " << device.getInfo<CL_DEVICE_DOUBLE_FP_CONFIG>() << "\n";
    std::cout << "Max work item sizes: " << printVector(device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()) << "\n";
    std::cout << "Max work item dimensions: " << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << "\n";
    std::cout << "Max work group size: " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << "\n";
    */
    context = std::make_unique<cl::Context>(device);


    std::filesystem::path kernelFileName{__FILE__};
    kernelFileName.replace_filename("UniverseKernel.cl");
    if (std::ifstream kernelFile{kernelFileName}; kernelFile) {
        std::ostringstream ss;
        ss << "#define FP " << CLFloatTypeGet<FP>::fpName << "\n";
        ss << "#define FP3 " << CLFloatTypeGet<FP>::fpName << "3" << "\n";
        ss << kernelFile.rdbuf();
        const std::string &str = ss.str();
        //std::cout << "\n" << str << "\n";
        sources.emplace_back(str.c_str(), str.size());
    }
    else {
        throw std::runtime_error{"Could not read kernel in file " + kernelFileName.string()};
    }

    program = cl::Program{*context, sources};
//    std::cout << "Building kernels...\n";
    try {
        program.build({device});
    }
    catch (cl::BuildError &e) {
        std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
        std::cout << e.getBuildLog().size() << "\n";
    }

    queue = std::make_unique<cl::CommandQueue>(*context, device, CL_QUEUE_PROFILING_ENABLE);

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

//    std::cout << " - localWorkSize: " << localWorkSize << ", globalWorkSize: " << globalWorkSize << "\n";

//    std::cout << "Size of mass values: " << massBuffer->getInfo<CL_MEM_SIZE>() << " bytes\n";
}
