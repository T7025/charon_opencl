#pragma OPENCL EXTENSION cl_khr_fp64 : enable

void kernel calcNextPositionDouble(
        global double3 *position, global const double3 *velocity,
        global const double3 *acceleration, const double timeStep, const int numBodies) {
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
    position[i] += velocity[i] * (double3)(timeStep) + acceleration[i] * (double3)(timeStep * timeStep * 0.5);
//    printf(" - after(%d): %f %f %f\n", i, position[i].x, position[i].y, position[i].z);
//    const double txtdiv2 = timeStep * timeStep / 2.0;
//    position[i].x += velocity[i].x * timeStep + acceleration[i].x * txtdiv2;
//    position[i].y += velocity[i].y * timeStep + acceleration[i].y * txtdiv2;
//    position[i].z += velocity[i].z * timeStep + acceleration[i].z * txtdiv2;
}

void kernel calcFirstAccelerationDouble(
        global const double *mass, global const double3 *position, global double3 *velocity, global double3 *acceleration,
        const double timeStep, const double softeningLength, const int numBodies) {

    const int gid = get_global_id(0);
    if (gid >= numBodies) {
        return;
    }
    const int lid = get_local_id(0);
    const int localSize = get_local_size(0);

    double3 acc = (double3)0;
    for (int j = 0; j < numBodies; ++j) {
        const double3 diff = position[j] - position[gid];
        const double temp = rsqrt(dot(diff, diff) + softeningLength * softeningLength);
        acc += diff * (double3)(mass[j] * pown(temp, 3));
    }
    velocity[gid] += acc * (double3)(timeStep);
    acceleration[gid] = acc;
}

void kernel calcAccelerationDouble(
        global const double *mass, global const double3 *position, global double3 *velocity, global double3 *acceleration,
        const double timeStep, const double softeningLength, const int numBodies) {

    const int gid = get_global_id(0);
    if (gid >= numBodies) {
        return;
    }
    const int lid = get_local_id(0);
    const int localSize = get_local_size(0);

    double3 acc = (double3)0;
    for (int j = 0; j < numBodies; ++j) {
        const double3 diff = position[j] - position[gid];
        const double temp = rsqrt(dot(diff, diff) + softeningLength * softeningLength);
        acc += diff * (double3)(mass[j] * pown(temp, 3));
    }
    velocity[gid] += (acceleration[gid] + acc) * (double3)(timeStep * 0.5);
    acceleration[gid] = acc;
//    printf("vel: %f %f %f\n", velocity[gid].x, velocity[gid].y, velocity[gid].z);


//    barrier(CLK_LOCAL_MEM_FENCE);

//    for (int offset = 1; offset < localSize; offset *= 2) {
//    }

    /*
    const int groupId = get_group_id(0);
    if (lid == 0) {
        double3 sum = (double3)0;
        for (int i = 0; i < localSize; ++i) {
            sum += localAcc[lid];
        }
        newAcc[groupId] = sum;
    }
    //*/
    // Update vel[gid], acc[gid]
}

// END OF NON-DUPLICATED CODE



void kernel calcNextPositionFloat(
        global float3 *position, global const float3 *velocity,
        global const float3 *acceleration, const float timeStep, const int numBodies) {
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
//printf(" - before(%d): %f %f %f\n", i, position[i].x, position[i].y, position[i].z);
position[i] += velocity[i] * (float3)(timeStep) + acceleration[i] * (float3)(timeStep * timeStep * 0.5);
//printf(" - after(%d): %f %f %f\n", i, position[i].x, position[i].y, position[i].z);
//    const float txtdiv2 = timeStep * timeStep / 2.0;
//    position[i].x += velocity[i].x * timeStep + acceleration[i].x * txtdiv2;
//    position[i].y += velocity[i].y * timeStep + acceleration[i].y * txtdiv2;
//    position[i].z += velocity[i].z * timeStep + acceleration[i].z * txtdiv2;
}

void kernel calcFirstAccelerationFloat(
        global const float *mass, global const float3 *position, global float3 *velocity, global float3 *acceleration,
const float timeStep, const float softeningLength, const int numBodies) {

const int gid = get_global_id(0);
if (gid >= numBodies) {
return;
}
const int lid = get_local_id(0);
const int localSize = get_local_size(0);

float3 acc = (float3)0;
for (int j = 0; j < numBodies; ++j) {
const float3 diff = position[j] - position[gid];
const float temp = rsqrt(dot(diff, diff) + softeningLength * softeningLength);
acc += diff * (float3)(mass[j] * pown(temp, 3));
}
velocity[gid] += acc * (float3)(timeStep);
acceleration[gid] = acc;
}

void kernel calcAccelerationFloat(
        global const float *mass, global const float3 *position, global float3 *velocity, global float3 *acceleration,
const float timeStep, const float softeningLength, const int numBodies) {

const int gid = get_global_id(0);
if (gid >= numBodies) {
return;
}
const int lid = get_local_id(0);
const int localSize = get_local_size(0);

float3 acc = (float3)0;
for (int j = 0; j < numBodies; ++j) {
const float3 diff = position[j] - position[gid];
const float temp = rsqrt(dot(diff, diff) + softeningLength * softeningLength);
acc += diff * (float3)(mass[j] * pown(temp, 3));
}
velocity[gid] += (acceleration[gid] + acc) * (float3)(timeStep * 0.5);
acceleration[gid] = acc;
//    printf("vel: %f %f %f\n", velocity[gid].x, velocity[gid].y, velocity[gid].z);


//    barrier(CLK_LOCAL_MEM_FENCE);

//    for (int offset = 1; offset < localSize; offset *= 2) {
//    }

/*
const int groupId = get_group_id(0);
if (lid == 0) {
    float3 sum = (float3)0;
    for (int i = 0; i < localSize; ++i) {
        sum += localAcc[lid];
    }
    newAcc[groupId] = sum;
}
//*/
// Update vel[gid], acc[gid]
}