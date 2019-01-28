//
// Created by thomas on 01/01/19.
//

#pragma once

#include <base/Universe.hpp>
#include <barnes-hut/Node.hpp>

template <typename FP>
class Universe<Algorithm::barnesHut, Platform::cpuSingleThread, FP> : public UniverseBase {
public:
    explicit Universe(Settings settings);

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override;

    void logInternalState(std::ostream &out) override;

private:
    void calcNextStep() override;

    void calculateFirstAcc();

    void calculateNextAcc();

    void calcNextPosition();

    Vec3<FP> calculateAcceleration(const Node<FP> &targetNode) const;

    void flattenTree();

    void buildTree();

    void sortTree();

    void scalePositions();

    void calcSFCIndices();

    void generateInternalNodes();

    void removeDuplicateInternalNodes();

    void establishParentChildRel();

    void calculateNodeData();

    std::vector<Node<FP>> tree;
    Vec3<FP> treeBoundingBox;
    Vec3<FP> offset;
    FP treeBoundingBoxSize;
    unsigned numBodies;
    const FP errorRate;
    bool treeIsBuilt = false;
};

template <typename FP>
std::ostream &operator<<(std::ostream &out, const std::vector<Node<FP>> &tree) {
    std::string tabSpace = "'";
    for (const auto &node : tree) {
        std::string tab;
        for (unsigned i = 0; i < node.getDepth(); ++i) {
            tab += tabSpace;
        }
        out << tab << node << "\n";
    }
    return out;
}

/*template <typename FP>
std::ostream &operator<<(std::ostream &out, const Universe<Algorithm::barnesHut, Platform::cpuSingleThread, FP> &tree) {
    std::string tabSpace = "'";
    for (const auto &node : tree.tree) {
        std::string tab;
        for (unsigned i = 0; i < node.getDepth(); ++i) {
            tab += tabSpace;
        }
        out << tab << node << "\n";
    }
    return out;
}*/
