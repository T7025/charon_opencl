//
// Created by thomas on 28/01/19.
//
// Adapted from Boost.Compute

#pragma once

#include <cstdint>
#include <CL/cl2.hpp>
#include <memory>
#include <cmath>

using Int = cl_long;
using UInt = cl_ulong;
using Float = cl_double;
using IntVec = cl_long2;
using UIntVec = cl_ulong2;
using FloatVec = cl_double2;
using Float3 = cl_double3;
//using size_t = uint64_t;

namespace detail {

UInt get_global_id(int i) {
    return 0;
}
UInt get_local_id(int i) {
    return 0;
}
UInt get_group_id(int i) {
    return 0;
}
UInt get_local_size(int i) {
    return 0;
}
UInt min(UInt i, UInt j) {
    return 0;
}
UInt max(UInt i, UInt j) {
    return 0;
}
#define CLK_LOCAL_MEM_FENCE 0
void barrier(int i) {}

template <typename KeyType>
bool COMPARE(KeyType lhs, KeyType rhs) {
    return 0;
}

template <typename KeyType>
void kernel_bitonic_block_sort(const UInt count, /*local*/ KeyType lkeys, /*global*/ KeyType keys_first) {
    // Work item global and local ids
    UInt gid = get_global_id(0);
    UInt lid = get_local_id(0);

    // declare my_key
    KeyType my_key;

    // load key
    if (gid < count) {
        my_key = keys_first[gid];
    }

    // load key and index to local memory
    lkeys[lid] = my_key;

    UInt offset = get_group_id(0) * get_local_size(0);
    UInt n = min((uint)(get_local_size(0)),(count - offset));

    // When work group size is a power of 2 bitonic sorter can be used;
    // otherwise, slower odd-even sort is used.
    // check if n is power of 2
    if(((n != 0) && ((n & (~n + 1)) == n))) {
        // bitonic sort, not stable
        // wait for keys to be stored in local memory
        barrier(CLK_LOCAL_MEM_FENCE);

        #pragma unroll
        for (UInt length = 1; length < n; length <<= 1) {
            // direction of sort: false -> asc, true -> desc
            bool direction = ((lid & (length << 1)) != 0);
            for (UInt k = length; k > 0; k >>= 1) {
                // sibling to compare with my key
                UInt sibling_idx = lid ^ k;
                KeyType sibling_key = lkeys[sibling_idx];
                bool compare = COMPARE(sibling_key, my_key);
                bool swap = compare ^ (sibling_idx < lid) ^ direction;
                my_key = swap ? sibling_key : my_key;

                barrier(CLK_LOCAL_MEM_FENCE);
                lkeys[lid] = my_key;
                barrier(CLK_LOCAL_MEM_FENCE);
            }
        }
        // end of bitonic sort
    }
    else {
        // odd-even sort, not stable
        bool lid_is_even = (lid % 2) == 0;
        UInt oddsibling_idx = (lid_is_even) ? max(lid,(uint)(1)) - 1 : min(lid+1,n-1);
        UInt evensibling_idx = (lid_is_even) ? min(lid+1,n-1) : max(lid,(uint)(1)) - 1;

        // wait for keys to be stored in local memory
        barrier(CLK_LOCAL_MEM_FENCE);

        #pragma unroll
        for (UInt i = 0; i < n; i++) {
            UInt sibling_idx = i%2 == 0 ? evensibling_idx : oddsibling_idx;
            KeyType sibling_key = lkeys[sibling_idx];
            bool compare = COMPARE(sibling_key, my_key);
            bool swap = compare ^ (sibling_idx < lid);
            my_key = swap ? sibling_key : my_key;

            barrier(CLK_LOCAL_MEM_FENCE);
            lkeys[lid] = my_key;
            barrier(CLK_LOCAL_MEM_FENCE);
        }
        // end of odd-even sort
    }
    // save key and value

    if (gid < count) {
       keys_first[gid] = my_key;
    }
}

template <typename KeyType>
size_t pick_bitonic_block_sort_block_size(size_t proposed_wg, size_t lmem_size) {
    size_t n = proposed_wg;

    size_t lmem_required = n * sizeof(KeyType);

    // try to force at least 4 work-groups of >64 elements
    // for better occupancy
    while(lmem_size < (lmem_required * 4) && (n > 64)) {
        n /= 2;
        lmem_required = n * sizeof(KeyType);
    }
    while(lmem_size < lmem_required && (n != 1)) {
        n /= 2;
        if(n < 1) n = 1;
        lmem_required = n * sizeof(KeyType);
    }

    if(n < 2)   { return 1; }
    else if(n < 4)   { return 2; }
    else if(n < 8)   { return 4; }
    else if(n < 16)  { return 8; }
    else if(n < 32)  { return 16; }
    else if(n < 64)  { return 32; }
    else if(n < 128) { return 64; }
    else if(n < 256) { return 128; }
    else             { return 256; }
}

template <typename KeyType, typename Compare>
inline size_t bitonic_block_sort(std::unique_ptr<cl::Buffer> keys_first,
                                 Compare compare,
                                 const size_t count,
                                 const cl::Program &program,
                                 const cl::Device &device,
                                 cl::CommandQueue &queue) {

    cl::KernelFunctor<UInt, cl::LocalSpaceArg, cl::Buffer> bitonic_block_sort_functor{program, "bitonic_block_sort"};
    size_t work_group_size = pick_bitonic_block_sort_block_size<KeyType>(
            bitonic_block_sort_functor.getKernel().getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device),
            device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>());
    size_t global_size = work_group_size * static_cast<size_t>(
            std::ceil(float(count) / work_group_size)
    );
    cl::EnqueueArgs eArgs{queue, cl::NDRange{global_size}, cl::NDRange{work_group_size}};
    bitonic_block_sort_functor(eArgs, count, cl::Local(sizeof(KeyType) * work_group_size), *keys_first);

    // return size of the block
    return work_group_size;
}

template <typename KeyType, typename Compare>
inline void merge_sort_on_gpu(KeyType first,
                             size_t count,
                             Compare compare,
                             const cl::Program &program,
                             const cl::Device &device,
                             cl::CommandQueue &queue) {
    if (count < 2) {
        return;
    }
    size_t block_size = bitonic_block_sort(first, compare, count, program, device, queue);

    // for small input size only block sort is performed
    if(count <= block_size) {
        return;
    }


}

}
