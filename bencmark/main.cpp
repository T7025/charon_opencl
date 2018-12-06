//
// Created by thomas on 12/4/18.
//
#include <vector>
#include <iostream>

template <typename FP>
std::vector<unsigned> getInterestingNumberOfBodies() {
    // lstopo (or lstopo-no-graphics) for overview
    // "getconf -a | grep CACHE" for cache sizes in bytes
    // Laptop:
    //   L1d: 32K
    //   L2: 256K
    //   L3: 3072K
    // Desktop:
    //   L1d: 32K
    //   L2: 4096K
    //   L3: 16384K
    // Studento:
    //   L1d: 16K
    //   L2: 2048K
    //   L3: 6144K
    // Fire:
    //   L1d: 32K
    //   L2: 256K
    //   L3: 30720K

    // 1 body in memory: (1 + 3 + 3 + 3) * sizeof(FP)
    // sizeof(FP) == 32 -> 10 * 32 bit/body -> 40 byte/body
    // sizeof(FP) == 64 -> 10 * 64 bit/body -> 80 byte/body

    // Cache             # bodies
    // KB    byte        float       double
    //    16       16384       409.6      204.8
    //    32       32768       819.2      409.6
    //   256      262144      6553.6     3276.8
    //  2048     2097152     52428.8    26214.4
    //  3072     3145728     78643.2    39321.6
    //  4096     4194304    104857.6    52428.8
    //  6144     6291456    157286.4    78643.2
    // 16384    16777216    419430.4   209715.2
    // 30720    31457280    786432     393216
    // Assumes that the Universe/Vector/... classes are 0 bytes.

}

int main() {
    // Create standard json settings (with lib)
    // For each implementation: run with same settings
    // Put results in timestamped files/folders
    // Benchmark init, copy (= read, logInternalState) and compute separately.
    // Important metrics: num bodies, run time for each algorithm, device, FP
    std::cout << "vector of floats: " << sizeof(std::vector<float>{}) << " bytes\n";
    std::cout << "vector of doubles: " << sizeof(std::vector<double>{}) << " bytes\n";
}