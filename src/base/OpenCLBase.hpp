//
// Created by thomas on 12/01/19.
//

#pragma once

#include <CL/cl2.hpp>
#include <memory>
#include <filesystem>

class OpenCLBase {
public:
    explicit OpenCLBase(const cl::Program::Sources &sources);

    static std::string getKernelSource(const std::filesystem::path &filePath);

protected:
    cl::Platform platform;
    cl::Device device;
    std::unique_ptr<cl::Context> context;
    cl::Program program;
    std::unique_ptr<cl::CommandQueue> queue;
};

