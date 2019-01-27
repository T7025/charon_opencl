//
// Created by thomas on 26/01/19.
//

#include <barnes-hut/SFCIndex.hpp>
#include <cassert>

bool lessMSB(const uintv x, const uintv y) {
    return x < y && x < (x ^ y);
}

constexpr auto clzToMsb = sizeof(uint64_t) * 8 - 1;  // 31 if a.x is 4 bytes, 64 if a.x is 8 bytes.

inline auto findMSBMain(const uint64_t x) {
//    return (__builtin_clzl(x) ^ clzToMsb) + (x != 0);  // If clz better implementation -> OK for entire 64 bit range.
    return (__builtin_clzl((x << 1) + 1) ^ clzToMsb);  // Used because result of __builtin_clzl(x) is undefined when x == 0.
}

bool comp(const SFCIndex &lhs, const SFCIndex &rhs) {
    bool result = lhs.x < rhs.x;
    uintv t1 = lhs.x ^ rhs.x;
    uintv t2 = lhs.y ^ rhs.y;
    if (lessMSB(t1, t2)) {
        result = lhs.y < rhs.y;
        t1 = t2;
    }
    t2 = lhs.z ^ rhs.z;
    if (lessMSB(t1, t2)) {
        result = lhs.z < rhs.z;
    }
    return result;
}

int main() {
    for (uintv i = 0; i < 5; ++i) {
        for (uintv j = 0; j < 5; ++j) {
            for (uintv l = 0; l < 5; ++l) {
                for (uintv m = 0; m < 5; ++m) {
                    for (uintv n = 0; n < 5; ++n) {
                        for (uintv i1 = 0; i1 < 5; ++i1) {
    SFCIndex a{i, j, l};
    SFCIndex b{m, n, i1};

    std::cout << a << "\n";
    std::cout << b << "\n";

//    std::cout << (a < b) << "\n";
//    std::cout << lessMSB(a.x, b.x) << ", " << lessMSB(a.y, b.y) << ", " << lessMSB(a.z, b.z) << "\n";
//
//    std::cout << findMSB(a.x) << ", " << findMSB(b.x) << "\n";
//    std::cout << findMSB(a.y) << ", " << findMSB(b.y) << "\n";
//    std::cout << findMSB(a.z) << ", " << findMSB(b.z) << "\n";
//
    /*a.x = ~uintv{0};

    uintv sfc1 = 0;
    uintv sfc[3] = {0, 0, 0};
    uintv mask = uintv{1} << 63;
    std::cout << "m: " << std::bitset<k>{mask} << "\n";

    uintv i = 0;
    while (i < k) {
        sfc[2] |= a.z & (1 << i / 3);
    }


    for (uintv i = 0; i < k - 2; ++i) {
        sfc1 |= a.x & mask;
        sfc1 |= (a.y & mask) >> 1;
        sfc1 |= (a.z & mask) >> 2;
        std::cout << "m: " << std::bitset<k>{mask} << "\n";
        std::cout << "sfc1: " << std::bitset<k>{sfc1} << "\n";
        mask >>= 1;
    }

    const auto msbAx = findMSB(a.x), msbAy = findMSB(a.y), msbAz = findMSB(a.z);
    const auto msbBx = findMSB(b.x), msbBy = findMSB(b.y), msbBz = findMSB(b.z);
    */
    /*auto comp = [&](){




        return (msbAx < msbBx || (msbAx == msbBx && (msbAy < msbBy || (msbAy == msbBy && msbAz < msbBz))));
    };*/

//    std::cout << comp() << "\n";
    if ((a < b) != comp(a, b)) {
        throw -1;
    }
//    assert((a < b) == (msbAx < msbBx || (msbAx == msbBx && (msbAy < msbBy || (msbAy == msbBy && msbAz < msbBz)))));
                        }
                    }
                }
            }
        }
    }
}