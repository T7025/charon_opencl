//
// Created by thomas on 12/4/18.
//
template <typename FP>
std::vector<unsigned> getInterestingNumberOfBodies() {
    // Laptop:
    //   L1d: 32K
    //   L2: 256K
    //   L3: 3072K
    // Desktop:
    //
    // Studento:
    //   L1d: 16K
    //   L2: 2048K
    //   L3: 6144K
    // Fire:
    //   L1d: 32K
    //   L2: 256K
    //   L3: 30720K

    // 1 body in memory: (1 + 3 + 3 + 3) * sizeof(FP)

}

int main() {
    // Create standard json settings (with lib)
    // For each implementation: run with same settings
    // Put results in timestamped files/folders
    // Benchmark init, copy (= read, logInternalState) and compute separately.
    // Important metrics: num bodies, run time for each algorithm, device, FP
}