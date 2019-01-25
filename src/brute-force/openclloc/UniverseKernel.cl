// FP and FP3 get defined at runtime.
//#define FP FP
//#define FP3 FP3

// To format:
// "\s(global|local|kernel)\s" -> "/*$1*/"
// Autoformat
// "/\*(global|local|kernel)\*/" -> " $1 "

#pragma OPENCL EXTENSION cl_khr_fp64 : enable


void kernel calcNextPosition(
         global FP3 *position,
         global const FP3 *velocity,
         global const FP3 *acceleration,
//                  local FP3 *localVelocity,
//                  local FP3 *localAcceleration,
                  const FP timeStep,
                  const FP sqrTimeStep,
                  const int numBodies
) {
    const int i = get_global_id(0);
    if (i >= numBodies) {
        return;
    }

//    local FP3 locVel[64];
//    event_t e = async_work_group_copy(locVel, velocity + get_group_id(0), 64, 0);
//    wait_group_events(1, &e);
//    const int lid = get_local_id(0);
//    const int localSize = get_local_size(0);
//    const int numBlocks = get_global_size(0) / localSize;
//
//    for (int blockIndex = 0; blockIndex < numBlocks; ++numBlocks) {
//        barrier(CLK_LOCAL_MEM_FENCE);
//
//    }
//    printf("%f, %f\n", velocity[i].x, locVel[get_local_id(0)].x);
    position[i] += velocity[i] * (FP3)(timeStep) + acceleration[i] * (FP3)(sqrTimeStep);
}

FP3 calcAcceleration(
         constant const FP *mass,
         global const FP3 *position,
         global FP3 *velocity,
         global FP3 *acceleration,
//         local FP *localMass,
         local FP3 *localPos,
                  const FP timeStep,
                  const FP softeningLength,
                  const int numBodies
) {

    const int gid = get_global_id(0);
    const int lid = get_local_id(0);
    const int localSize = get_local_size(0);
    const int numBlocks = get_global_size(0) / localSize;

    const FP sqrSofteningLength = pown(softeningLength, 2);

    const FP3 gidPosition = position[gid];
    FP3 acc = (FP3) 0;

    for (int blockIndex = 0; blockIndex < numBlocks; ++blockIndex) {
        const int numLocalBodies = select(localSize, numBodies % localSize, blockIndex == numBlocks - 1);
//        localMass[lid] = mass[blockIndex * localSize + lid];
        localPos[lid] = position[blockIndex * localSize + lid];

        barrier(CLK_LOCAL_MEM_FENCE);
        for (int j = 0; j < numLocalBodies; ++j) {
//        for (int j = 0; j < localSize; ++j) {
            const FP3 diff = localPos[j] - gidPosition;
            const FP temp = rsqrt(dot(diff, diff) + sqrSofteningLength);
            acc += diff * (FP3)(mass[blockIndex * localSize + lid] * pown(temp, 3));
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    return acc;
}

void kernel calcFirstVecAndAcc(
         constant const FP *mass,
         global const FP3 *position,
         global FP3 *velocity,
         global FP3 *acceleration,
//         local FP *localMass,
         local FP3 *localPos,
                  const FP timeStep,
                  const FP softeningLength,
                  const int numBodies
) {
    const int gid = get_global_id(0);
    if (gid >= numBodies) {
        return;
    }
    FP3 acc = calcAcceleration(mass, position, velocity, acceleration, localPos, timeStep, softeningLength,
                               numBodies);
    velocity[gid] += acc * (FP3)(timeStep);
    acceleration[gid] = acc;
}

void kernel calcVecAndAcc(
         constant const FP *mass,
         global const FP3 *position,
         global FP3 *velocity,
         global FP3 *acceleration,
//         local FP *localMass,
         local FP3 *localPos,
                  const FP timeStep,
                  const FP softeningLength,
                  const int numBodies
) {

    const int gid = get_global_id(0);
    if (gid >= numBodies) {
        return;
    }
    FP3 acc = calcAcceleration(mass, position, velocity, acceleration, localPos, timeStep, softeningLength,
                               numBodies);

    velocity[gid] += (acceleration[gid] + acc) * (FP3)(timeStep * 0.5);
    acceleration[gid] = acc;
}

