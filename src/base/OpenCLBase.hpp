//
// Created by thomas on 12/01/19.
//

#pragma once

#include <CL/cl2.hpp>
#include <filesystem>
#include <memory>
#include <vector>
#include <settings/Settings.hpp>

template <typename FP> struct CLFloatTypeGet {};
template <> struct CLFloatTypeGet<double> {
    typedef double value_type;
    typedef cl_double3 value_type3;
    typedef cl_double4 cl_fp_vec;
    static constexpr const char *fpName = "double";
    static constexpr const char *fpVecName = "double4";
    static constexpr const char *uintName = "ulong";
    static constexpr const char *uintVecName = "ulong4";
};
template <> struct CLFloatTypeGet<float> {
    typedef float value_type;
    typedef cl_float3 value_type3;
    typedef cl_float2 cl_fp_vec;
    static constexpr const char *fpName = "float";
    static constexpr const char *fpVecName = "float2";
    static constexpr const char *uintName = "uint";
    static constexpr const char *uintVecName = "uint2";
};

class OpenCLBase {
public:
    explicit OpenCLBase(const cl::Program::Sources &sources, const Settings &settings);

    static std::string getKernelSource(const std::filesystem::path &filePath);

    template <typename T>
    std::unique_ptr<cl::Buffer> bufferFromVector(std::vector<T> &&vector, cl_mem_flags flags) {
        return std::make_unique<cl::Buffer>(*context, flags | CL_MEM_COPY_HOST_PTR,
                                            vector.size() * sizeof(T), vector.data());
    }

protected:
    cl::Platform platform;
    cl::Device device;
    std::unique_ptr<cl::Context> context;
    cl::Program program;
    std::unique_ptr<cl::CommandQueue> queue;
};


std::ostream &operator<<(std::ostream &out, const cl_double3 &val);

std::ostream &operator<<(std::ostream &out, const cl_float3 &val);
