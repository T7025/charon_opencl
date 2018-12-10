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
template <> struct CLFloatTypeGet<double>{
    typedef cl_double value_type;
    typedef cl_double3 value_type3;
    static constexpr const char *kernelSuffix = "Double";
};
template <> struct CLFloatTypeGet<float>{
    typedef cl_float value_type;
    typedef cl_float3 value_type3;
    static constexpr const char *kernelSuffix = "Float";
};

std::ostream &operator<<(std::ostream &out, const cl_double3 &val) {
    out << val.x << ',' << val.y << ',' << val.z;
    return out;
}

std::ostream &operator<<(std::ostream &out, const cl_float3 &val) {
    out << val.x << ',' << val.y << ',' << val.z;
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

        out << "mass,xPos,yPos,zPos,xVel,yVel,zVel,xAcc,yAcc,zAcc\n";
        for (unsigned i = 0; i < mass.size(); ++i) {
            out << mass[i] << ',' << position[i] << ',' << velocity[i] << ',' << acceleration[i] << '\n';
        }
    }

    void step(unsigned int numSteps) override {
        for (unsigned step = 0; step < numSteps; ++step) {
//            calcNextPosition();
            cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl_fp> calcNextPosition(program, std::string{"calcNextPosition"} + CLFloatTypeGet<FP>::kernelSuffix);
            cl::EnqueueArgs eArgs(*queue, cl::NullRange, cl::NDRange(mass.size()), cl::NullRange);
            calcNextPosition(eArgs, *positionBuffer, *velocityBuffer, *accelerationBuffer, settings.timeStep);

//            clGetKernelWorkGroupInfo()

            for (unsigned i = 0; i < mass.size(); ++i) {
//                auto newAcceleration = calcAcceleration(i);
//                velocity[i] += newAcceleration * settings.timeStep;
//                acceleration[i] = newAcceleration;
            }
        }
    }


private:
    /*void calcNextPosition() {
        for (unsigned i = 0; i < mass.size(); ++i) {
            position[i] +=
                    velocity[i] * settings.timeStep + acceleration[i] * settings.timeStep * settings.timeStep / 2;
        }
    }

    Vec3<FP> calcAcceleration(const unsigned int target) const {
        Vec3<FP> newAcceleration{0, 0, 0};
        for (unsigned j = 0; j < mass.size(); ++j) {
//            if (target == j) continue;
            const Vec3<FP> diff = position[j] - position[target];
//            FP norm = diff.norm();
//            newAcceleration += diff * (mass[j] / (norm * norm * norm));
            FP temp = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z +
                           settings.softeningLength * settings.softeningLength);
            newAcceleration += diff * (mass[j] / (temp * temp * temp));

        }
        return newAcceleration;
    }*/

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
//    const auto clDeviceType = CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU;
    const auto clDeviceType = CL_DEVICE_TYPE_GPU;

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

    std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
    std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

    context = std::make_unique<cl::Context>(device);


    std::filesystem::path kernelFileName{__FILE__};
    kernelFileName.replace_filename("UniverseKernel.cl");
    if (std::ifstream kernelFile{kernelFileName}; kernelFile) {
        std::ostringstream ss;
        ss << kernelFile.rdbuf();
        const std::string &str = ss.str();
        std::cout << "\n" << str << "\n";
        sources.emplace_back(str.c_str(), str.size());
    }
    else {
        throw std::runtime_error{"Could not read kernel in file " + kernelFileName.string()};
    }

    program = cl::Program{*context, sources};
    std::cout << "Building kernels...\n";
    try {
        program.build({device});
    }
    catch (cl::BuildError &e) {
        std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
        std::cout << e.getBuildLog().size() << "\n";
    }

    queue = std::make_unique<cl::CommandQueue>(*context, device);
/*
    //------------------ Below is experimental


    cl::Buffer buffer_A(*context, CL_MEM_READ_WRITE, sizeof(int) * 10);
    cl::Buffer buffer_B(*context, CL_MEM_READ_WRITE, sizeof(int) * 10);
    cl::Buffer buffer_C(*context, CL_MEM_READ_WRITE, sizeof(int) * 10);

    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

    cl::CommandQueue queue(*context, device);

    queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 10, A);
    queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * 10, B);

    //run the kernel
//    cl::KernelFunctor simple_add(cl::Kernel(program,"simple_add"),queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
//    simple_add(buffer_A,buffer_B,buffer_C);

//    auto simple_add = cl::make_kernel<cl::Buffer&, cl::Buffer&, cl::Buffer&>(program, "simple_add");
//    cl::EnqueueArgs eargs(queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
//    simple_add(eargs, buffer_A,buffer_B,buffer_C).wait();

    //alternative way to run the kernel
    //
//    cl::Kernel kernel_add=cl::Kernel(program,"simple_add");
//    kernel_add.setArg(0,buffer_A);
//    kernel_add.setArg(1,buffer_B);
//    kernel_add.setArg(2,buffer_C);
//    queue.enqueueNDRangeKernel(kernel_add,cl::NullRange,cl::NDRange(10),cl::NullRange);
//    queue.finish();
    //

//    auto kernel = cl::make_kernel<cl::Buffer&, cl::Buffer&, cl::Buffer&>(program, "simple_add");
//    cl::EnqueueArgs eArgs(queue, cl::NullRange, cl::NDRange(10), cl::NullRange);
//    kernel(eArgs, buffer_A, buffer_B, buffer_C);

    cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> simple_add(program, "simple_add");
    cl::EnqueueArgs eArgs(queue, cl::NullRange, cl::NDRange(10), cl::NullRange);
    simple_add(eArgs, buffer_A, buffer_B, buffer_C);


    int C[10];
    //read result C from the device to array C
    queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * 10, C);


    queue.finish();

    std::cout << " result: \n";
    for (int i = 0; i < 10; i++) {
        std::cout << C[i] << " ";
    }
    //*/



    std::cout << "\n";
    std::cout << "Double supported?: " << (CL_DEVICE_DOUBLE_FP_CONFIG > 0) << "\n";

    for (unsigned i = 0; i < settings.numberOfBodies; ++i) {
        auto[m, pos, vel] = bodyGenerator->getBody();
        mass.emplace_back(cl_fp(m));
        position.emplace_back(cl_fp3{cl_fp(pos.x), cl_fp(pos.y), cl_fp(pos.z)});
        velocity.emplace_back(cl_fp3{cl_fp(vel.x), cl_fp(vel.y), cl_fp(vel.z)});
        acceleration.emplace_back(cl_fp3{0, 0, 0});
    }

    massBuffer = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                              sizeof(FP) * mass.size(), mass.data());
    positionBuffer = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                                  sizeof(FP) * position.size() * 4, position.data());
    velocityBuffer = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                                  sizeof(FP) * velocity.size() * 4, velocity.data());
    accelerationBuffer = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                                      sizeof(FP) * acceleration.size() * 4, acceleration.data());
//    massBuffer = std::make_unique<cl::Buffer>(mass.begin(), mass.end(), true);  // 3d argument: read only?
//    positionBuffer = std::make_unique<cl::Buffer>(position.begin(), position.end(), false);
//    velocityBuffer = std::make_unique<cl::Buffer>(velocity.begin(), velocity.end(), false);
//    accelerationBuffer = std::make_unique<cl::Buffer>(acceleration.begin(), acceleration.end(), false);

    std::cout << "Size of mass values: " << massBuffer->getInfo<CL_MEM_SIZE>() << " bytes\n";

    queue->enqueueWriteBuffer(*massBuffer, CL_TRUE, 0, sizeof(FP) * mass.size(), mass.data());

    /*
    std::vector<typename CLFloatTypeGet<FP>::value_type> t;


    if constexpr(sizeof(FP) == sizeof(double)) {
        std::vector<cl_double3> tempPos;
        for (const auto &p : position) {
            tempPos.emplace_back(cl_double3{p.x, p.y, p.z});
        }
        queue->enqueueWriteBuffer(*positionBuffer, CL_TRUE, 0, sizeof(cl_double3) * tempPos.size(), tempPos.data());

    }
    else if constexpr(sizeof(FP) == sizeof(float)) {
        std::vector<cl_float3> tempPos;
        for (const auto &p : position) {
            tempPos.emplace_back(cl_float3{p.x, p.y, p.z});
        }
        queue->enqueueWriteBuffer(*positionBuffer, CL_TRUE, 0, sizeof(cl_float3) * tempPos.size(), tempPos.data());
    }
*/
}
