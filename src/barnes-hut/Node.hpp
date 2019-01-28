//
// Created by thomas on 12/01/19.
//

#pragma once

#include "SFCIndex.hpp"
#include <base/Vec3.hpp>

//constexpr unsigned k = 64;  // Max tree depth.
//using uintv = uint64_t;

template <typename FP>
class Node {
public:
    Node() : sfcIndex{}, depth{}, mass{-1}, position{}, data{} {};

    Node(const Vec3<FP> &pos, const Vec3<FP> &vel, const Vec3<FP> &acc, const FP mass) :
            sfcIndex{}, depth{k}, mass{mass}, position{pos}, data{vel, acc} {}

//    const Vec3<FP> &getPosition() const;
//    Vec3<FP> &getPosition();
//    const SFCIndex &getSFCIndex() const;
//    SFCIndex &getSFCIndex();
//    const unsigned &getDepth() const;
//    unsigned &getDepth();
//    const intv *getChildren() const;
//    intv *getChildren();
//    const FP &getMass() const;
//    FP &getMass();
//    const Vec3<FP> &getVelocity() const;
//    Vec3<FP> &getVelocity();
//    const Vec3<FP> &getAcceleration() const;
//    Vec3<FP> &getAcceleration();
//
//    bool isLeaf() const;
//    bool isParentOf(const Node &other) const;
//
//    bool operator<(const Node &other) const;
//    bool operator==(const Node &other) const;
    const Vec3<FP> &getPosition() const {
        return position;
    }

    Vec3<FP> &getPosition() {
        return position;
    }

    const SFCIndex &getSFCIndex() const {
        return sfcIndex;
    }

    SFCIndex &getSFCIndex() {
        return sfcIndex;
    }

    bool operator==(const Node &other) const {
        return sfcIndex == other.sfcIndex && depth == other.depth;
    }

    const unsigned &getDepth() const {
        return depth;
    }

    unsigned &getDepth() {
        return depth;
    }

    const intv *getChildren() const {
        return data.children;
    }

    intv *getChildren() {
        return data.children;
    }

    const FP &getMass() const {
        return mass;
    }

    FP &getMass() {
        return mass;
    }

    const Vec3<FP> &getVelocity() const {
        return data.leafData.vel;
    }

    Vec3<FP> &getVelocity() {
        return data.leafData.vel;
    }

    const Vec3<FP> &getAcceleration() const {
        return data.leafData.acc;
    }

    Vec3<FP> &getAcceleration() {
        return data.leafData.acc;
    }

    bool isLeaf() const {
        return depth == k;
    }

    bool isParentOf(const Node &other) const {
        if (depth < other.depth) {
            auto shift = k - depth;
            // If xor of (depth) most significant bits is zero (=they are equal),
            // then this Node is a parent of other Node.
            return depth == 0 || (((sfcIndex.x ^ other.sfcIndex.x) >> shift) == 0
                                  && ((sfcIndex.y ^ other.sfcIndex.y) >> shift) == 0
                                  && ((sfcIndex.z ^ other.sfcIndex.z) >> shift) == 0);
        }
        return false;
    }

    bool operator<(const Node &other) const {
        return sfcIndex < other.sfcIndex || (other.sfcIndex == sfcIndex && depth > other.depth);
    }

private:
    SFCIndex sfcIndex;
    unsigned depth;
    FP mass;
    Vec3<FP> position;

    union Data {
        Data() : children{-1, -1, -1, -1, -1, -1, -1, -1} {}

        Data(const Vec3<FP> &vel, const Vec3<FP> &acc) : leafData{vel, acc} {}

        intv children[8];
        struct {
            Vec3<FP> vel;
            Vec3<FP> acc;
        } leafData;
    } data;
};

struct Node2 {

};

template <typename FP>
std::ostream &operator<<(std::ostream &out, const Node<FP> &node) {
    out << node.getSFCIndex() << "; " << node.getDepth() << "; " << node.getMass() << "; " << node.getPosition();
    if (node.getDepth() < k) {
        out << "; " << node.getChildren()[0];
        for (int i = 1; i < 8; ++i) {
            out << "," << node.getChildren()[i];
        }
    }
    return out;
}


