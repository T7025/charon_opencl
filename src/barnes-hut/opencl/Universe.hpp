//
// Created by thomas on 27/01/19.
//

#pragma once

#include <base/Util.hpp>
#include <base/Universe.hpp>
#include <base/OpenCLBase.hpp>
#include <CL/cl2.hpp>
//namespace CLKer{
//template <typename FP>
//using FP2 = FP;
//#include <barnes-hut/opencl/Node.cl>
//}

template <typename FP>
struct CLTypeGet {};

template <>
struct CLTypeGet<float> {
    using Int = cl_int;
    using UInt = cl_uint;
    using Float = cl_float;
    using IntVec = cl_int4;
    using UIntVec = cl_uint4;
    using FloatVec = cl_float4;
    using Float3 = cl_float3;
    static constexpr const char *clCompileOpts =
            "k=32;Int=int;UInt=uint;Float=float;IntVec=int4;UIntVec=uint4;FloatVec=float4;Float3=float3";
};
template <>
struct CLTypeGet<double> {
    using Int = cl_long;
    using UInt = cl_ulong;
    using Float = cl_double;
    using IntVec = cl_long2;
    using UIntVec = cl_ulong2;
    using FloatVec = cl_double2;
    using Float3 = cl_double3;
    static constexpr const char *clCompileOpts =
            "k=64;Int=long;UInt=ulong;Float=double;IntVec=long2;UIntVec=ulong2;FloatVec=double2;Float3=double3";
};


template <typename FP>
class Universe<Algorithm::barnesHut, Platform::openCL, FP> : public UniverseBase, public OpenCLBase {
public:
    explicit Universe(Settings settings);

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override;

    void logInternalState(std::ostream &out) override;

    void finish() override;

private:
    using Int = typename CLTypeGet<FP>::Int;
    using UInt = typename CLTypeGet<FP>::UInt;
    using Float = typename CLTypeGet<FP>::Float;
    using Float3 = typename CLTypeGet<FP>::Float3;

    struct SFCIndex {
        UInt x, y, z;
    };

    struct Node {
        SFCIndex sfcIndex;
        UInt depth;
        Float3 position;
        Float mass;

        union {
            Int children[8];
            struct {
                Float3 velocity, acceleration;
            } leafData;
        } data;
    };

    void calcNextStep() override;

    void calculateFirstAcc();

    void calculateNextAcc();

    void calcNextPosition();

//    Vec3<FP> calculateAcceleration(const Node &targetNode) const;

    void flattenTree();

    void buildTree();

    void sortTree();

    void scalePositions();

    void calcSFCIndices();

    void generateInternalNodes();

    void removeDuplicateInternalNodes();

    void establishParentChildRel();

    void calculateNodeData();

    std::vector<FP> mass;
    std::vector<Float3> position;
    std::vector<Float3> velocity;
    std::vector<Float3> acceleration;

    unsigned localWorkSize;
    unsigned globalWorkSize;
    std::unique_ptr<cl::EnqueueArgs> eArgs;

    // Used to transfer initial condition to device and results to host.
    std::unique_ptr<cl::Buffer> massBuffer;
    std::unique_ptr<cl::Buffer> positionBuffer;
    std::unique_ptr<cl::Buffer> velocityBuffer;
    std::unique_ptr<cl::Buffer> accelerationBuffer;

    // Contains data only red from/written to on device.
    std::unique_ptr<cl::Buffer> treeBuffer;  // All Nodes (max size: 4 * numBodies - 3 Nodes)
    std::unique_ptr<cl::Buffer> bodyIndices;  // Used to decrease divergence. Maps i to ith Body (=leaf Node) in treeBuffer (numBodies UInts)
    UInt treeSizeHost;  // Current size of the tree
    std::unique_ptr<cl::Buffer> treeSize;  // Current size of the tree (1 UInt)
    std::unique_ptr<cl::Buffer> treeBoundingBox;  // 3D bounding box of bodies (3 Floats)
    std::unique_ptr<cl::Buffer> offset;  // Used to calculate SFC indices (3 Floats)
    bool treeIsBuilt = false;

//    cl::KernelFunctor<cl::Buffer, int, int, cl::LocalSpaceArg> sortSFC;
//    cl::KernelFunctor<cl::Buffer, int, int, cl::LocalSpaceArg> sortDepth;
//    cl::KernelFunctor<cl::Buffer, int, int, cl::LocalSpaceArg> sortSFCAndDepth;
};
