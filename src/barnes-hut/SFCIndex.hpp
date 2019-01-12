//
// Created by thomas on 12/01/19.
//
#pragma once

#include <bitset>
#include <iostream>

constexpr unsigned k = 64;  // Max tree depth.
using uintv = uint64_t;
using intv = int64_t;

class SFCIndex {
public:
    bool operator==(const SFCIndex &other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator<(const SFCIndex &other) const {
        unsigned j = 0;
        uintv t1 = 0;
        uintv t2 = x ^other.x;
        if (lessMSB(t1, t2)) {
            j = 0;
            t1 = t2;
        }
        t2 = y ^ other.y;
        if (lessMSB(t1, t2)) {
            j = 1;
            t1 = t2;
        }
        t2 = z ^ other.z;
        if (lessMSB(t1, t2)) {
            j = 2;
        }
        bool result;
        switch (j) {
            case 0:
                result = x < other.x;
                break;
            case 1:
                result = y < other.y;
                break;
            case 2:
                result = z < other.z;
                break;
        }
        return result;
    }

    uintv x, y, z;
private:
    /// Checks if the most significant bit of x is less than the most significant bit of y
    bool lessMSB(const uintv x, const uintv y) const {
        return x < y && x < (x ^ y);
    }
};

std::ostream &operator<<(std::ostream &out, const SFCIndex &sfcIndex);
