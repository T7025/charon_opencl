//
// Created by thomas on 27/01/19.
//

#include "Universe.hpp"
#include <filesystem>
//#include <boost/compute/algorithm/sort.hpp>
//#include <boost/compute/core.hpp>


template class Universe<Algorithm::barnesHut, Platform::openCL, float>;
template class Universe<Algorithm::barnesHut, Platform::openCL, double>;

template <typename FP>
using UniverseImpl = Universe<Algorithm::barnesHut, Platform::openCL, FP>;

template <typename FP>
UniverseImpl<FP>::Universe(Settings settings) :
    UniverseBase{settings},
    OpenCLBase{[]() {
        std::filesystem::path sfcIndexKernelFN{__FILE__};
        sfcIndexKernelFN.replace_filename("Node.cl");
        std::filesystem::path nodeKernelFN{__FILE__};
        nodeKernelFN.replace_filename("Node.cl");
        std::filesystem::path universeKernelFN{__FILE__};
        universeKernelFN.replace_filename("UniverseKernel.cl");
        const cl::Program::Sources sources{
            OpenCLBase::getKernelSource(sfcIndexKernelFN),
            OpenCLBase::getKernelSource(nodeKernelFN),
            OpenCLBase::getKernelSource(universeKernelFN)
        };
        return sources;
    }(), this->settings, CLTypeGet<FP>::clCompileOpts} {}

template <typename FP>
void UniverseImpl<FP>::init(std::unique_ptr<BodyGenerator> bodyGenerator) {
    for (unsigned i = 0; i < settings.numberOfBodies; ++i) {
        auto[m, pos, vel] = bodyGenerator->getBody();
        mass.emplace_back(Float(m));
        position.emplace_back(Float3{Float(pos.x), Float(pos.y), Float(pos.z)});
        velocity.emplace_back(Float3{Float(vel.x), Float(vel.y), Float(vel.z)});
        acceleration.emplace_back(Float3{0, 0, 0});
    }

    massBuffer = bufferFromVector(std::move(mass), CL_MEM_READ_ONLY | CL_MEM_HOST_READ_ONLY);
    positionBuffer = bufferFromVector(std::move(position), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    velocityBuffer = bufferFromVector(std::move(velocity), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);
    accelerationBuffer = bufferFromVector(std::move(acceleration), CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY);

    treeBuffer = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY,
                                            (settings.numberOfBodies * 4 - 3) * sizeof(Node));
    treeSizeHost = 0;
    treeSize = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, sizeof(UInt));
    treeBoundingBox = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, 3 * sizeof(Float));
    offset = std::make_unique<cl::Buffer>(*context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, 3 * sizeof(Float));

    localWorkSize = 64;
    globalWorkSize = getGlobWorkSize(settings.numberOfBodies, localWorkSize);

    eArgs = std::make_unique<cl::EnqueueArgs>(
            *queue,
            cl::NullRange,
            cl::NDRange(globalWorkSize),
            cl::NDRange(localWorkSize)
    );
}

template <typename FP>
void UniverseImpl<FP>::logInternalState(std::ostream &out) {
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

template <typename FP>
void UniverseImpl<FP>::finish() {
    queue->flush();
}

template <typename FP>
void UniverseImpl<FP>::calcNextStep() {
    if (doneFirstStep) {
        if (!doneFirstAccCalc) {
            calculateFirstAcc();
        } else {
            calculateNextAcc();
        }
    }
    calcNextPosition();
}

template <typename FP>
void UniverseImpl<FP>::calculateFirstAcc() {
    buildTree();

    // Call getBodyIndices kernel. GlobalSize ~ treeSize
    // Call calcFirstAcceleration kernel. GlobalSize ~ settings.numberOfBodies.
//    for (unsigned i = 0; i < tree.size(); ++i) {
//        if (tree[i].isLeaf()) {
//            Vec3<FP> newAcceleration = calculateAcceleration(tree[i]);
//            tree[i].getVelocity() += newAcceleration * settings.timeStep;
//            tree[i].getAcceleration() = newAcceleration;
//        }
//    }
    flattenTree();
}

template <typename FP>
void UniverseImpl<FP>::calculateNextAcc() {
    buildTree();
    // Call getBodyIndices kernel. GlobalSize ~ treeSize
    // Call calcAcceleration kernel. GlobalSize ~ settings.numberOfBodies
    flattenTree();
}

template <typename FP>
void UniverseImpl<FP>::calcNextPosition() {
    // Call calcNextPosition. GlobalSize ~ numBodies
}

//template <typename FP>
//Vec3<FP> UniverseImpl<FP>::calculateAcceleration(const Node &targetNode) const {
//
//}

template <typename FP>
void UniverseImpl<FP>::flattenTree() {
    sortTree();  // less: return lhs.depth > rhs.depth
    // Move all non-Body Nodes to back
    treeSizeHost = settings.numberOfBodies;
    treeIsBuilt = false;
}

template <typename FP>
void UniverseImpl<FP>::buildTree() {
    scalePositions();
    calcSFCIndices();
//    sortSFC(*treeBuffer, *treeSize, 0, cl::Local(sizeof()));
    generateInternalNodes();
    sortTree();
    removeDuplicateInternalNodes();
    establishParentChildRel();
    // Update treeSize here (async, can wait untill after calculateNodeData)
    //treeSize =
    calculateNodeData();
    treeIsBuilt = true;
}

template <typename FP>
void UniverseImpl<FP>::sortTree() {
//    boost::compute::context ctx = context->get();
//
//    boost::compute::vector<int> nodes(10);
//
//    auto buffer = nodes.get_buffer();

//    boost::compute::sort()
}

template <typename FP>
void UniverseImpl<FP>::scalePositions() {

}

template <typename FP>
void UniverseImpl<FP>::calcSFCIndices() {

}

template <typename FP>
void UniverseImpl<FP>::generateInternalNodes() {

}

template <typename FP>
void UniverseImpl<FP>::removeDuplicateInternalNodes() {

}

template <typename FP>
void UniverseImpl<FP>::establishParentChildRel() {

}

template <typename FP>
void UniverseImpl<FP>::calculateNodeData() {

}
