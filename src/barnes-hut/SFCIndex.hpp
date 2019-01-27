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

template <typename UIntVec>
struct SFCIndex2 {
    UIntVec x, y, z;
};

template <typename UIntVec>
auto operator==(const SFCIndex2<UIntVec> &lhs, const SFCIndex2<UIntVec> &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

inline auto findMSB(const uintv x) {
    constexpr auto clzToMsb = sizeof(uintv) * 8 - 1;
    // Used because result of __builtin_clzl(x) is undefined when x == 0.
    return (__builtin_clzl((x << 1) + 1) ^ clzToMsb);
    // If clz better implementation -> OK for entire 64 bit range.
//    return (__builtin_clzl(x) ^ clzToMsb) + (x != 0);
}

namespace detail {
template <typename UIntVec>
inline auto lessMSB(const UIntVec x, const UIntVec y) {
    return x < y && x < (x ^ y);
}
}

template <typename UIntVec>
auto operator<(const SFCIndex2<UIntVec> &lhs, const SFCIndex2<UIntVec> &rhs) {
    bool result = lhs.x < rhs.x;
    UIntVec t1 = lhs.x ^ rhs.x;
    UIntVec t2 = lhs.y ^ rhs.y;
    if (detail::lessMSB(t1, t2)) {
        result = lhs.y < rhs.y;
        t1 = t2;
    }
    t2 = lhs.z ^ rhs.z;
    if (detail::lessMSB(t1, t2)) {
        result = lhs.z < rhs.z;
    }
    return result;
}

