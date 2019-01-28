//
// Created by thomas on 12/01/19.
//

#include <sstream>
#include <iostream>
#include <fstream>
#include "OpenCLBase.hpp"

OpenCLBase::OpenCLBase(const cl::Program::Sources &sources, const Settings &settings, const std::string &compileOpts) {
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

    program = cl::Program{*context, sources};
    //std::cout << "Building kernels...\n";
    try {
        program.build({device}, (settings.openclCompileOpts + (compileOpts.empty() ? "":";") + compileOpts).c_str());
    }
    catch (cl::BuildError &e) {
        std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
        std::cout << e.getBuildLog().size() << "\n";
    }

    queue = std::make_unique<cl::CommandQueue>(*context, device, CL_QUEUE_PROFILING_ENABLE);
}

std::string OpenCLBase::getKernelSource(const std::filesystem::path &filePath) {
    if (std::ifstream kernelFile{filePath}; kernelFile) {
        std::ostringstream ss;
        ss << kernelFile.rdbuf();
        return ss.str();
    }
    else {
        throw std::runtime_error{"Could not read kernel in file " + filePath.string()};
    }
}

std::ostream &operator<<(std::ostream &out, const cl_double3 &val) {
    out << val.x << ' ' << val.y << ' ' << val.z;
    return out;
}

std::ostream &operator<<(std::ostream &out, const cl_float3 &val) {
    out << val.x << ' ' << val.y << ' ' << val.z;
    return out;
}
