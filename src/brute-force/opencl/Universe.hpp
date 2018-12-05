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
#include <CL/cl.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>


template <typename FP>
class Universe<Algorithm::bruteForce, Platform::openCL, FP> : public UniverseBase {
public:
    explicit Universe(Settings settings) : UniverseBase{std::move(settings)} {};

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override;


    void logInternalState(std::ostream &out) override {
        assert(mass.size() == position.size() && mass.size() == velocity.size() && mass.size() == acceleration.size());

        out << "mass,xPos,yPos,zPos,xVel,yVel,zVel,xAcc,yAcc,zAcc\n";
        for (unsigned i = 0; i < mass.size(); ++i) {
            out << mass[i] << ',' << position[i] << ',' << velocity[i] << ',' << acceleration[i] << '\n';
        }
    }

    void step(unsigned int numSteps) override {
        for (unsigned step = 1; step < numSteps; ++step) {
            calcNextPosition();
            for (unsigned i = 0; i < mass.size(); ++i) {
                auto newAcceleration = calcAcceleration(i);
                velocity[i] += newAcceleration * settings.timeStep;
                acceleration[i] = newAcceleration;
            }
        }
    }


private:
    void calcNextPosition() {
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
    }

    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::Program::Sources sources;
    cl::Program program;


    std::vector<FP> mass;
    std::vector<Vec3<FP>> position;
    std::vector<Vec3<FP>> velocity;
    std::vector<Vec3<FP>> acceleration;
};


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

    context = cl::Context{device};

    std::filesystem::path kernelFileName{__FILE__};
    kernelFileName.replace_filename("UniverseKernel.cl");
    if (std::ifstream kernelFile{kernelFileName}; kernelFile) {
        std::ostringstream ss;
        ss << kernelFile.rdbuf();
        const std::string &str = ss.str();
        std::cout << "\n" << str << "\n";
        sources.emplace_back(str.c_str(), str.size());
    } else {
        throw std::runtime_error{"Could not read kernel in file " + kernelFileName.string()};
    }

    program = cl::Program{context, sources};
    std::cout << "Building kernels...\n";
    if (program.build({device}) != CL_SUCCESS) {
        std::cout<<" Error building: "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)<<"\n";
        throw std::runtime_error{" Error building: " + program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)};
    }
    std::cout<<"Kernel build log: "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)<<"\n";

    //------------------ Below is experimental


    cl::Buffer buffer_A(context,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_B(context,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_C(context,CL_MEM_READ_WRITE,sizeof(int)*10);

    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

    cl::CommandQueue queue(context, device);

    queue.enqueueWriteBuffer(buffer_A,CL_TRUE,0,sizeof(int)*10,A);
    queue.enqueueWriteBuffer(buffer_B,CL_TRUE,0,sizeof(int)*10,B);

    //run the kernel
//    cl::KernelFunctor simple_add(cl::Kernel(program,"simple_add"),queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
//    simple_add(buffer_A,buffer_B,buffer_C);
    auto simple_add = cl::make_kernel<cl::Buffer&, cl::Buffer&, cl::Buffer&>(program, "simple_add");
    cl::EnqueueArgs eargs(queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
    simple_add(eargs, buffer_A,buffer_B,buffer_C).wait();

    //alternative way to run the kernel
    /*
    cl::Kernel kernel_add=cl::Kernel(program,"simple_add");
    kernel_add.setArg(0,buffer_A);
    kernel_add.setArg(1,buffer_B);
    kernel_add.setArg(2,buffer_C);
    queue.enqueueNDRangeKernel(kernel_add,cl::NullRange,cl::NDRange(10),cl::NullRange);
    queue.finish();
    //*/

    int C[10];
    //read result C from the device to array C
    queue.enqueueReadBuffer(buffer_C,CL_TRUE,0,sizeof(int)*10,C);


    queue.finish();

    std::cout<<" result: \n";
    for(int i=0;i<10;i++){
        std::cout<<C[i]<<" ";
    }

    for (unsigned i = 0; i < settings.numberOfBodies; ++i) {
        auto[m, pos, vel] = bodyGenerator->getBody();
        mass.emplace_back(m);
        position.emplace_back(pos);
        velocity.emplace_back(vel);
        acceleration.emplace_back(0, 0, 0);
    }

}
