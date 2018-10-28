#include <omp.h>
#include <CL/cl.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    nlohmann::json j;
    j["pi"] = 3.14;
    j["list"] = {1, 2, 3};
    std::cout << j << "\n";
    std::cout << "Max threads (OpenMP): " << omp_get_max_threads() << std::endl;


    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);

    if (all_platforms.empty()) {
        std::cout << " No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    std::cout << "Available OpenCL platform(s): " << all_platforms[0].getInfo<CL_PLATFORM_NAME>();
    for (unsigned i = 1; i < all_platforms.size(); ++i) {
        std::cout << ", " << all_platforms[i].getInfo<CL_PLATFORM_NAME>();
    }
}