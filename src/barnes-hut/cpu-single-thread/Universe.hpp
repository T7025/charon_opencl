//
// Created by thomas on 01/01/19.
//

#pragma once

#include <base/Universe.hpp>
#include <barnes-hut/Node.hpp>

template <typename T, typename S>
T &lshift(T &val, const S &shift) {
    val <<= shift - (shift > 0);
    val <<= shift > 0;
    return val;
};

template <typename T, typename S>
T lshift(T &&val, const S &shift) {
    val <<= shift - (shift > 0);
    val <<= shift > 0;
    return val;
};

template <typename T, typename S>
T &lshift(T &val, S &&shift) {
    val <<= shift - (shift > 0);
    val <<= shift > 0;
    return val;
};

template <typename T, typename S>
T lshift(T &&val, S &&shift) {
    val <<= shift - (shift > 0);
    val <<= shift > 0;
    return val;
};

template <typename T, typename S>
T &rshift(T &val, const S &shift) {
    val >>= shift - (shift > 0);
    val >>= shift > 0;
    return val;
};

template <typename T, typename S>
T rshift(T &&val, const S &shift) {
    val >>= shift - (shift > 0);
    val >>= shift > 0;
    return val;
};

template <typename T, typename S>
T &rshift(T &val, S &&shift) {
    val >>= shift - (shift > 0);
    val >>= shift > 0;
    return val;
};

template <typename T, typename S>
T rshift(T &&val, S &&shift) {
    val >>= shift - (shift > 0);
    val >>= shift > 0;
    return val;
};

template <typename FP>
class Universe<Algorithm::barnesHut, Platform::cpuSingleThread, FP> : public UniverseBase {
public:
    explicit Universe(Settings settings) : UniverseBase{settings}, errorRate{(FP) settings.barnesHutCutoff} {};
    using UniverseBase::UniverseBase;

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override {
        for (unsigned i = 0; i < settings.numberOfBodies; ++i) {
            auto[m, pos, vel] = bodyGenerator->getBody();
            tree.emplace_back(Vec3<FP>{pos}, Vec3<FP>{vel}, Vec3<FP>{0, 0, 0}, (FP) m);
        }
        numBodies = (unsigned) tree.size();
        tree.reserve(4 * numBodies - 3);
    }

    void logInternalState(std::ostream &out) override {
        assert(!treeIsBuilt);

        out << "mass xPos yPos zPos xVel yVel zVel xAcc yAcc zAcc\n";
        for (unsigned i = 0; i < numBodies; ++i) {
            out << tree[i].getMass() << " " << tree[i].getPosition() << " " << tree[i].getVelocity() << " "
                << tree[i].getAcceleration() << "\n";
        }
    }

    void step(unsigned int numSteps) override {
        if (!doneFirstStep) {
            calcNextPosition();
            calculateFirstAcc();
            doneFirstStep = true;
            numSteps--;
        }

        for (unsigned step = 0; step < numSteps; ++step) {
            calcNextPosition();
            calculateNextAcc();
        }
    }

    void calculateFirstAcc() {
//        calcNextPosition();

        buildTree();
//    std::cout <<*this<<"\n";
        for (unsigned i = 0; i < tree.size(); ++i) {
            if (tree[i].isLeaf()) {
                Vec3<FP> newAcceleration = calculateAcceleration(tree[i]);
                tree[i].getVelocity() += newAcceleration * settings.timeStep;
                tree[i].getAcceleration() = newAcceleration;
            }
        }

        flattenTree();

//        calcNextPosition();
    }

    void calculateNextAcc() {
//    auto start = omp_get_wtime();
        buildTree();
//    std::cout << *this <<"\n";
        for (unsigned i = 0; i < tree.size(); ++i) {
            if (tree[i].isLeaf()) {
                Vec3<FP> newAcceleration = calculateAcceleration(tree[i]);
                tree[i].getVelocity() += (tree[i].getAcceleration() + newAcceleration) * settings.timeStep / 2;
                tree[i].getAcceleration() = newAcceleration;
            }
        }

        flattenTree();
//        calcNextPosition();
    }

    void calcNextPosition() {
        assert(!treeIsBuilt);

        const auto size = numBodies;
        for (unsigned i = 0; i < size; ++i) {
            auto &pos = tree[i].getPosition();
            const auto &vel = tree[i].getVelocity();
            const auto &acc = tree[i].getAcceleration();
            pos += vel * settings.timeStep + acc * (settings.timeStep * settings.timeStep / 2);
        }
    }

    Vec3<FP> calculateAcceleration(const Node<FP> &targetNode) const {
        Vec3<FP> result = {0, 0, 0};

        std::vector<unsigned> stack;
        stack.reserve(k);
        stack.emplace_back(tree.size() - 1);
        while (!stack.empty()) {
            const auto &curNode = tree[stack.back()];
            stack.pop_back();

            const auto isFarEnough = [&]() {
                if (errorRate == 0) {
                    return false;
                }
                const auto diff = curNode.getPosition() - targetNode.getPosition();
                const auto scale = FP(1) / (uintv(1) << curNode.getDepth());
                const auto x = (diff.x * treeBoundingBox.y * treeBoundingBox.z);
                const auto y = (treeBoundingBox.x * diff.y * treeBoundingBox.z);
                const auto z = (treeBoundingBox.x * treeBoundingBox.y * diff.z);
                const auto r = (treeBoundingBox.x * treeBoundingBox.y * treeBoundingBox.z) * scale / errorRate;
                const auto res = (x * x + y * y + z * z) >= (r * r);
                return res;
            };
            /*auto isFarEnough2 = [&]() {
                auto radius = std::max(treeBoundingBox.x, std::max(treeBoundingBox.y, treeBoundingBox.z));
                return radius * radius / curNode.getPosition().squareDistance(targetNode.getPosition()) <
                       errorRate * errorRate;
            };*/
            if (curNode.isLeaf() && targetNode.getPosition() == curNode.getPosition()) {
                continue;
            }
            else if ((curNode.isLeaf() || isFarEnough()) && !curNode.isParentOf(targetNode)) {
                const Vec3<FP> diff = curNode.getPosition() - targetNode.getPosition();
                FP temp = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z +
                               settings.softeningLength * settings.softeningLength);
                result += diff * (curNode.getMass() / (temp * temp * temp));
                continue;
            }
            else {
                for (unsigned i = 0; i < 8; ++i) {
                    if (curNode.getChildren()[i] < 0) {
                        continue;
                    }
                    stack.emplace_back(curNode.getChildren()[i]);
                }
            }
        }
//    std::cout << "result: " << result <<"\n";
        return result;
    }

    void flattenTree() {
        assert(treeIsBuilt);
        std::sort(tree.begin(), tree.end(),
                  [](const Node<FP> &lhs, const Node<FP> &rhs) { return lhs.getDepth() > rhs.getDepth(); });
        tree.resize(numBodies);
        treeIsBuilt = false;
    }

    void buildTree() {
//    std::cout << *this <<"\n";

        scalePositions();
//    std::cout << "Scaled positions" << std::endl;
//    std::cout << treeBoundingBox << std::endl;
//    std::cout << *this <<"\n";
        calcSFCIndices();
//    std::cout << "Calculated SFCIndices" << std::endl;
//    std::cout << *this <<"\n";

        for (unsigned i = 0; i < tree.size() - 1; ++i) {
            if ((tree[i] == tree[i + 1])) {
                std::cout << tree[i] << "\n" << tree[i + 1] << "\n";
                assert(!(tree[i] == tree[i + 1]));
            }
        }

        generateInternalNodes();
//    std::cout << "Generated internal nodes" << std::endl;
//    std::cout << *this <<"\n";
        removeDuplicateInternalNodes();
//    std::cout << "Removed duplicate internal nodes" << std::endl;
//    std::cout << *this <<"\n";
        establishParentChildRel();
//    std::cout << "Established parent-child relation" << std::endl;
//    std::cout << *this <<"\n";
        calculateNodeData();
//    std::cout << "Calculated node data" << std::endl;
//    std::cout << *this <<"\n";
        treeIsBuilt = true;
    }

    void sortTree() {
        std::sort(tree.begin(), tree.end());
    }

    void scalePositions() {
        Vec3<FP> min = {
                std::numeric_limits<FP>::infinity(),
                std::numeric_limits<FP>::infinity(),
                std::numeric_limits<FP>::infinity()
        };
        Vec3<FP> max = {
                -std::numeric_limits<FP>::infinity(),
                -std::numeric_limits<FP>::infinity(),
                -std::numeric_limits<FP>::infinity()
        };
        const auto size = tree.size();
        for (unsigned i = 0; i < size; ++i) {
            min.x = std::min(min.x, tree[i].getPosition().x);
            min.y = std::min(min.y, tree[i].getPosition().y);
            min.z = std::min(min.z, tree[i].getPosition().z);
            max.x = std::max(max.x, tree[i].getPosition().x);
            max.y = std::max(max.y, tree[i].getPosition().y);
            max.z = std::max(max.z, tree[i].getPosition().z);
        }
        offset = min;
        //treeBoundingBoxSize = std::max(max.x, std::max(max.y, max.z));
        treeBoundingBox = max - min;
        treeBoundingBox.x = std::max(treeBoundingBox.x, std::numeric_limits<FP>::min());
        treeBoundingBox.y = std::max(treeBoundingBox.y, std::numeric_limits<FP>::min());
        treeBoundingBox.z = std::max(treeBoundingBox.z, std::numeric_limits<FP>::min());
    }

    void calcSFCIndices() {
        const FP mult = std::pow(2, k);
        for (unsigned i = 0; i < tree.size(); ++i) {
            const auto pos = tree[i].getPosition() - offset;
            tree[i].getSFCIndex().x = uintv(mult * (pos.x / treeBoundingBox.x)) - uintv(pos.x == treeBoundingBox.x);
            tree[i].getSFCIndex().y = uintv(mult * (pos.y / treeBoundingBox.y)) - uintv(pos.y == treeBoundingBox.y);
            tree[i].getSFCIndex().z = uintv(mult * (pos.z / treeBoundingBox.z)) - uintv(pos.z == treeBoundingBox.z);
        }
    }

    void generateInternalNodes() {
        sortTree();

        auto treeSize = tree.size();
        tree.resize(2 * treeSize - 1);

        for (unsigned i = 0; i < treeSize - 1; ++i) {
            // Count leading zeroes long long
            auto xorX = __builtin_clzll(tree[i].getSFCIndex().x ^ tree[i + 1].getSFCIndex().x);
            auto xorY = __builtin_clzll(tree[i].getSFCIndex().y ^ tree[i + 1].getSFCIndex().y);
            auto xorZ = __builtin_clzll(tree[i].getSFCIndex().z ^ tree[i + 1].getSFCIndex().z);
            auto depth = xorX < xorY ? xorX : xorY;
            depth = depth < xorZ ? depth : xorZ;
            depth = k - unsigned(sizeof(long long) * 8 - depth);

            typedef decltype(tree[i].getSFCIndex().x) sfcType;
            sfcType mask = ~sfcType{0};
            mask >>= depth;  // depth < k <= sizeof(uintv) * 8 -> OK, no need of rshift()
            mask >>= sizeof(mask) * 8 - k;  // sizeof(mask) * 8 - k < sizeof(mask) * 8 -> OK

            tree[treeSize + i].getSFCIndex().x = tree[i].getSFCIndex().x | mask;
            tree[treeSize + i].getSFCIndex().y = tree[i].getSFCIndex().y | mask;
            tree[treeSize + i].getSFCIndex().z = tree[i].getSFCIndex().z | mask;
            tree[treeSize + i].getDepth() = depth;
        }
    }

    void removeDuplicateInternalNodes() {
        sortTree();
        tree.erase(std::unique(tree.begin(), tree.end()), tree.end());
    }

    void establishParentChildRel() {
        auto treeSize = tree.size();

        tree.resize(2 * treeSize - 1);

        for (unsigned i = 0; i < treeSize - 1; ++i) {
            auto shift = k - std::min(tree[i].getDepth(), tree[i + 1].getDepth());
            auto xorX = __builtin_clzll(rshift(tree[i].getSFCIndex().x ^ tree[i + 1].getSFCIndex().x, shift));
            auto xorY = __builtin_clzll(rshift(tree[i].getSFCIndex().y ^ tree[i + 1].getSFCIndex().y, shift));
            auto xorZ = __builtin_clzll(rshift(tree[i].getSFCIndex().z ^ tree[i + 1].getSFCIndex().z, shift));
            auto depth = xorX < xorY ? xorX : xorY;
            depth = depth < xorZ ? depth : xorZ;
            depth = k - unsigned(sizeof(long long) * 8 - depth);
            depth -= shift;

            typedef decltype(tree[i].getSFCIndex().x) sfcType;
            sfcType mask = ~sfcType{0};
            //mask >>= depth;
            rshift(mask, depth);
            mask >>= sizeof(mask) * 8 - k;

            tree[treeSize + i].getSFCIndex().x = tree[i].getSFCIndex().x | mask;
            tree[treeSize + i].getSFCIndex().y = tree[i].getSFCIndex().y | mask;
            tree[treeSize + i].getSFCIndex().z = tree[i].getSFCIndex().z | mask;
            tree[treeSize + i].getDepth() = depth;
            tree[treeSize + i].getChildren()[0] = i;
        }

        auto nodeCompWChildren = [](const Node<FP> &lhs, const Node<FP> &rhs) {
            return lhs < rhs || (lhs == rhs && lhs.getChildren()[0] < rhs.getChildren()[0]);
        };

        std::sort(tree.begin() + treeSize, tree.end(), nodeCompWChildren);

        for (unsigned i = treeSize; i < tree.size(); ++i) {
            if (i == tree.size() - 1 || !(tree[i] == tree[i + 1])) {
                unsigned j;
                for (j = 1; j < 8; ++j) {
                    if (tree[i] == tree[i - j] && i - j >= treeSize) {
                        tree[i].getChildren()[j] = tree[i - j].getChildren()[0];
                    }
                    else {
                        break;
                    }
                }
                const auto parentIndex = tree[i].getChildren()[0] + 1;
                // 0 < j < 8
                for (unsigned k = 0; k < j; ++k) {
                    tree[parentIndex].getChildren()[k] = tree[i].getChildren()[j - 1 - k];
                }
            }
        }
//    std::cout << *this <<"\n";
        tree.resize(treeSize);
//    std::cout << *this <<"\n";
    }

    void calculateNodeData() {
        for (unsigned i = 0; i < tree.size(); ++i) {
            if (tree[i].getDepth() != k) {
                FP mass = 0;
                Vec3<FP> position = {0, 0, 0};
                for (unsigned j = 0; j < 8; ++j) {
                    // If parallel: check tree[tree[i].getChildren()[j]].getMass() >= 0 to see if
                    // child data is already calculated
                    if (tree[i].getChildren()[j] < 0) {
                        break;
                    }
                    const auto &child = tree[tree[i].getChildren()[j]];
                    mass += child.getMass();
                    position += child.getPosition() * child.getMass();
                }
                position /= mass;
                tree[i].getMass() = mass;
                tree[i].getPosition() = position;
            }
        }
    }


private:
    std::vector<Node<FP>> tree;
    Vec3<FP> treeBoundingBox;
    Vec3<FP> offset;
    FP treeBoundingBoxSize;
    unsigned numBodies;
    const FP errorRate;
    bool treeIsBuilt = false;
};

template <typename FP>
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
}
