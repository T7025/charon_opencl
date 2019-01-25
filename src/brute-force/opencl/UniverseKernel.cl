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
    if (i >= numBodies) {
        return;
    }
    position[i] += velocity[i] * (FP3)(timeStep) + acceleration[i] * (FP3)(timeStep * timeStep * 0.5);
}


FP3 calcAcceleration(
        global const FP *mass,
        global const FP3 *position,
        global FP3 *velocity,
        global FP3 *acceleration,
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

void kernel calcFirstVecAndAcc(
         global const FP *mass,
         global const FP3 *position,
         global FP3 *velocity,
         global FP3 *acceleration,
                  const FP timeStep,
                  const FP softeningLength,
                  const int numBodies
) {
    const int gid = get_global_id(0);
    if (gid >= numBodies) {
        return;
    }
    FP3 acc = calcAcceleration(mass, position, velocity, acceleration, timeStep, softeningLength, numBodies);
    velocity[gid] += acc * (FP3)(timeStep);
    acceleration[gid] = acc;
}

void kernel calcVecAndAcc(
         global const FP *mass,
         global const FP3 *position,
         global FP3 *velocity,
         global FP3 *acceleration,
                  const FP timeStep,
                  const FP softeningLength,
                  const int numBodies
) {

    const int gid = get_global_id(0);
    if (gid >= numBodies) {
        return;
    }
    FP3 acc = calcAcceleration(mass, position, velocity, acceleration, timeStep, softeningLength, numBodies);

    velocity[gid] += (acceleration[gid] + acc) * (FP3)(timeStep * 0.5);
    acceleration[gid] = acc;
}

