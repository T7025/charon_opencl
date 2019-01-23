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
                  const FP timeStep,
                  const int numBodies
) {
    const int i = get_global_id(0);
//    if (i < 129) {
//        printf("%i ", get_group_id(0));
//    }
//    if (i == 0) {
//        printf("%i\n", get_local_size(0));
//    }
    if (i >= numBodies) {
//printf("skip %i\n", i);
        return;
    }
//    printf(" - before(%d): %f %f %f\n", i, position[i].x, position[i].y, position[i].z);
    position[i] += velocity[i] * (FP3)(timeStep) + acceleration[i] * (FP3)(
            timeStep * timeStep
            * 0.5);
//    printf(" - after(%d): %f %f %f\n", i, position[i].x, position[i].y, position[i].z);
//    const FP txtdiv2 = timeStep * timeStep / 2.0;
//    position[i].x += velocity[i].x * timeStep + acceleration[i].x * txtdiv2;
//    position[i].y += velocity[i].y * timeStep + acceleration[i].y * txtdiv2;
//    position[i].z += velocity[i].z * timeStep + acceleration[i].z * txtdiv2;
}


FP3 calcAccelerationNaive(
        global const FP *mass,
        global const FP3 *position,
        global FP3 *velocity,
        global FP3 *acceleration,
        local FP *localMass,
        local FP3 *localPos,
        const FP timeStep,
        const FP softeningLength,
        const int numBodies
) {
    const int gid = get_global_id(0);
    FP3 acc = (FP3) 0;
    for (int j = 0; j < numBodies; ++j) {
        const FP3 diff = position[j] - position[gid];
        const FP temp = rsqrt(dot(diff, diff) + softeningLength * softeningLength);
        acc += diff *(FP3)(mass[j] * pown(temp, 3));
    }
    return acc;
}

FP3 calcAcceleration(
         global const FP *mass,
         global const FP3 *position,
         global FP3 *velocity,
         global FP3 *acceleration,
         local FP *localMass,
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
        localMass[lid] = mass[blockIndex * localSize + lid];
        localPos[lid] = position[blockIndex * localSize + lid];

        barrier(CLK_LOCAL_MEM_FENCE);
        for (int j = 0; j < numLocalBodies; ++j) {
            const FP3 diff = localPos[j] - gidPosition;
            const FP temp = rsqrt(dot(diff, diff) + sqrSofteningLength);
            acc += diff * (FP3)(localMass[j] * pown(temp, 3));
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    return acc;
}

void kernel calcFirstVecAndAcc(
         global const FP *mass,
         global const FP3 *position,
         global FP3 *velocity,
         global FP3 *acceleration,
         local FP *localMass,
         local FP3 *localPos,
                  const FP timeStep,
                  const FP softeningLength,
                  const int numBodies
) {
    const int gid = get_global_id(0);
    if (gid >= numBodies) {
        return;
    }
    FP3 acc = calcAcceleration(mass, position, velocity, acceleration, localMass, localPos, timeStep, softeningLength,
                               numBodies);
    velocity[gid] += acc * (FP3)(timeStep);
    acceleration[gid] = acc;
}

void kernel calcVecAndAcc(
         global const FP *mass,
         global const FP3 *position,
         global FP3 *velocity,
         global FP3 *acceleration,
         local FP *localMass,
         local FP3 *localPos,
                  const FP timeStep,
                  const FP softeningLength,
                  const int numBodies
) {

    const int gid = get_global_id(0);
    if (gid >= numBodies) {
        return;
    }
    FP3 acc = calcAcceleration(mass, position, velocity, acceleration, localMass, localPos, timeStep, softeningLength,
                               numBodies);

    velocity[gid] += (acceleration[gid] + acc) * (FP3)(timeStep * 0.5);
    acceleration[gid] = acc;
}
